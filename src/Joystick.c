/*
** $Id$
** Library to use joysticks with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"
#include "Joystick.h"

#define tojoystickp(L) \
	(SDL_Joystick **)luaL_checkudata(L, 1, "scrupp.joystick")

static SDL_Joystick *tojoystick(lua_State *L) {
	SDL_Joystick **joystick = tojoystickp(L);
	if (*joystick == NULL) {
		luaL_error(L, "attempt to use a closed joystick");
	}
	return *joystick;
}

static int Lua_Joystick_getNumber(lua_State *L) {
	lua_pushinteger(L, SDL_NumJoysticks());
	return 1;
}

static int Lua_Joystick_getNameForIndex(lua_State *L) {
	int index = luaL_checkint(L, 1);
	/* pushes nil if name==NULL */
	lua_pushstring(L, SDL_JoystickName(index-1));
	return 1;
}

static int Lua_Joystick_open(lua_State *L) {
	int index = luaL_checkint(L, 1);
	SDL_Joystick **ptr;
	SDL_Joystick *joystick = SDL_JoystickOpen(index-1);
	
	if (joystick == NULL) {
		lua_pushnil(L);
		lua_pushstring(L, SDL_GetError());
		return 2;
	}

	ptr = (SDL_Joystick **)lua_newuserdata(L, sizeof(SDL_Joystick *));
	*ptr = joystick;
	
	luaL_getmetatable(L, "scrupp.joystick");
	lua_setmetatable(L, -2);
	
	lua_pushliteral(L, "Scrupp:joystick_table");
	lua_rawget(L, LUA_REGISTRYINDEX);
	lua_pushvalue(L, -2);
	lua_rawseti(L, -2, index-1);
	lua_pop(L, 1);
	
	return 1;
}

static int Lua_Joystick_getName(lua_State *L) {
	SDL_Joystick *joystick = tojoystick(L);
	/* pushes nil if name==NULL */
	lua_pushstring(L, SDL_JoystickName(SDL_JoystickIndex(joystick)));
	return 1;
}

static int Lua_Joystick_getIndex(lua_State *L) {
	SDL_Joystick *joystick = tojoystick(L);
	lua_pushinteger(L, SDL_JoystickIndex(joystick)+1);
	return 1;
}

static int Lua_Joystick_getNumberOfAxes(lua_State *L) {
	SDL_Joystick *joystick = tojoystick(L);
	lua_pushinteger(L, SDL_JoystickNumAxes(joystick));
	return 1;
}

static int Lua_Joystick_getNumberOfBalls(lua_State *L) {
	SDL_Joystick *joystick = tojoystick(L);
	lua_pushinteger(L, SDL_JoystickNumBalls(joystick));
	return 1;
}

static int Lua_Joystick_getNumberOfHats(lua_State *L) {
	SDL_Joystick *joystick = tojoystick(L);
	lua_pushinteger(L, SDL_JoystickNumHats(joystick));
	return 1;
}

static int Lua_Joystick_getNumberOfButtons(lua_State *L) {
	SDL_Joystick *joystick = tojoystick(L);
	lua_pushinteger(L, SDL_JoystickNumButtons(joystick));
	return 1;
}

static int Lua_Joystick_getAxis(lua_State *L) {
	SDL_Joystick *joystick = tojoystick(L);
	int axis = luaL_checkint(L, 2);
	lua_pushinteger(L, SDL_JoystickGetAxis(joystick, axis-1));
	return 1;
}

static int Lua_Joystick_getHat(lua_State *L) {
	SDL_Joystick *joystick = tojoystick(L);
	int hat = luaL_checkint(L, 2);
	int dx = 0;
	int dy = 0;
	int hatPosition = SDL_JoystickGetHat(joystick, hat-1);
	if (hatPosition & SDL_HAT_LEFT) {
		dx = -1;
	} else if (hatPosition & SDL_HAT_RIGHT) {
		dx = 1;
	}
	if (hatPosition & SDL_HAT_UP) {
		dy = -1;
	} else if (hatPosition & SDL_HAT_DOWN) {
		dy = 1;
	}	
	lua_pushinteger(L, dx);
	lua_pushinteger(L, dy);
	return 2;
}

static int Lua_Joystick_getBall(lua_State *L) {
	SDL_Joystick *joystick = tojoystick(L);
	int ball = luaL_checkint(L, 2);
	int dx, dy;
	if (SDL_JoystickGetBall(joystick, ball, &dx, &dy) == -1) {
		lua_pushnil(L);
		lua_pushstring(L, SDL_GetError());
		return 2;
	}
	
	lua_pushinteger(L, dx);
	lua_pushinteger(L, dy);
	return 2;
}

static int Lua_Joystick_getButton(lua_State *L) {
	SDL_Joystick *joystick = tojoystick(L);
	int button = luaL_checkint(L, 2);
	lua_pushboolean(L, SDL_JoystickGetButton(joystick, button-1));
	return 1;
}

static int joystick_gc(lua_State *L) {
	/* stack(-1): joystick userdata to free */	
	SDL_Joystick **joystick = tojoystickp(L);
	if (*joystick != NULL) {
		lua_pushliteral(L, "Scrupp:joystick_table");
		lua_rawget(L, LUA_REGISTRYINDEX);
		lua_pushnil(L);
		lua_rawseti(L, -2, SDL_JoystickIndex(*joystick));
		
		SDL_JoystickClose(*joystick);
		*joystick = NULL;
	}
	return 0;
}

static int joystick_tostring(lua_State *L) {
	SDL_Joystick *joystick = *tojoystickp(L);
	if (joystick == NULL) {
		lua_pushliteral(L, "Joystick (closed)");
	} else {
		lua_pushfstring(L, "Joystick (%p)", joystick);
	}
	return 1;
}

static const struct luaL_Reg joysticklib [] = {
	{"getJoystickCount", Lua_Joystick_getNumber},
	{"getJoystickName", Lua_Joystick_getNameForIndex},
	{"openJoystick", Lua_Joystick_open},
	{NULL, NULL}
};

static const struct luaL_Reg joysticklib_m [] = {
	{"__gc", joystick_gc},
	{"__tostring", joystick_tostring},
	{"getName", Lua_Joystick_getName},
	{"getIndex", Lua_Joystick_getIndex},
	{"getNumberOfAxes", Lua_Joystick_getNumberOfAxes},
	{"getNumberOfBalls", Lua_Joystick_getNumberOfBalls},
	{"getNumberOfHats", Lua_Joystick_getNumberOfHats},
	{"getNumberOfButtons", Lua_Joystick_getNumberOfButtons},
	{"getAxis", Lua_Joystick_getAxis},
	{"getHat", Lua_Joystick_getHat},
	{"getBall", Lua_Joystick_getBall},
	{"isDown", Lua_Joystick_getButton},
	{"close", joystick_gc},
	{NULL, NULL}
};

int luaopen_joystick(lua_State *L, const char *parent) {
	/* create a table in the registry containing the mappings from
	   device indices to joystick objects */
	lua_newtable(L);
	lua_setfield(L, LUA_REGISTRYINDEX, "Scrupp:joystick_table");
	
	luaL_newmetatable(L, "scrupp.joystick");
	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);	/* duplicates the metatable */
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, joysticklib_m);
	lua_pop(L, 1);	/* pop the metatable */
	luaL_register(L, parent, joysticklib);
	return 1;
}
