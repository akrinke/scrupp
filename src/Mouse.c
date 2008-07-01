/*
** $Id$
** Library to use the mouse with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"
#include "Mouse.h"

#include <string.h>

const char* buttonNames [] = {
	"left",
	"middle",
	"right",
	"wheelUp",
	"wheelDown"
};

static int Lua_Mouse_getX(lua_State *L) {
	int x;
	SDL_GetMouseState(&x, NULL);
	lua_pushinteger(L, x);
	return 1;
}

static int Lua_Mouse_getY(lua_State *L) {
	int y;
	SDL_GetMouseState(NULL, &y);
	lua_pushinteger(L, y);
	return 1;
}

static int Lua_Mouse_getPos(lua_State *L) {
	int x, y;
	SDL_GetMouseState(&x, &y);
	lua_pushinteger(L, x);
	lua_pushinteger(L, y);
	return 2;
}

static int Lua_Mouse_isDown(lua_State *L) {
	const char *button = luaL_checkstring(L, -1);
	Uint8 bitmask = SDL_GetMouseState(NULL, NULL);
	if (strcmp(button, buttonNames[0]) == 0)
		bitmask &= SDL_BUTTON(SDL_BUTTON_LEFT);
	else if (strcmp(button, buttonNames[1]) == 0)
		bitmask &= SDL_BUTTON(SDL_BUTTON_MIDDLE);
	else if (strcmp(button, buttonNames[2]) == 0)
		bitmask &= SDL_BUTTON(SDL_BUTTON_RIGHT);
	else
		return luaL_error(L, "Unknown option to Mouse:isDown()", button);
	lua_pushboolean(L, bitmask);
	return 1;
}

static const struct luaL_Reg mouselib [] = {
	{"getX", Lua_Mouse_getX},
	{"getY", Lua_Mouse_getY},
	{"getPos", Lua_Mouse_getPos},
	{"isDown", Lua_Mouse_isDown},
	{NULL, NULL}
};

int luaopen_mouse(lua_State *L, const char *parent) {
	luaL_register(L, parent, mouselib);
	return 1;
}
