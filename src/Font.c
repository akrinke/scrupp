/*
** $Id$
** Library to use TrueType fonts with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"
#include "Macros.h"
#include "Font.h"
#include "Graphics.h"
#include "physfsrwops.h"

#include <SDL_ttf.h>

#define checkfont(L) \
	(TTF_Font **)luaL_checkudata(L, 1, "scrupp.font")

static int Lua_Font_load(lua_State *L) {
	TTF_Font *font;
	SDL_RWops *src;
	TTF_Font **ptr;
	const char* filename = luaL_checkstring(L, 1);
	int size = luaL_checkint(L, 2);
	src = PHYSFSRWOPS_openRead(filename);
	if (!src)
		return luaL_error(L, "Error loading font '%s': %s", filename, SDL_GetError());
	font = TTF_OpenFontRW( src, 1, size );
	if (!font)
		return luaL_error(L, "Error loading font '%s': %s", filename, TTF_GetError());
	/* new userdata for pointer to font */
	ptr = lua_newuserdata(L, sizeof(TTF_Font*));
	*ptr = font;
	luaL_getmetatable(L, "scrupp.font");
	lua_setmetatable(L, -2);
	return 1;
}

static int Lua_Font_getTextSize(lua_State *L) {
	TTF_Font **font_ptr = checkfont(L);
	const char *text = luaL_checkstring(L, 2);
	int w, h;
	TTF_SizeText(*font_ptr, text, &w, &h);
	lua_pushinteger(L, w);
	lua_pushinteger(L, h);
	return 2;
}

static int Lua_Font_getFontHeight(lua_State *L) {
	TTF_Font **font_ptr = checkfont(L);
	lua_pushinteger(L, TTF_FontHeight(*font_ptr));
	return 1;
}

static int Lua_Font_getFontLineSkip(lua_State *L) {
	TTF_Font **font_ptr = checkfont(L);
	lua_pushinteger(L, TTF_FontLineSkip(*font_ptr));
	return 1;
}

static int Lua_Font_generateImage(lua_State *L) {
	TTF_Font **font_ptr = checkfont(L);
	const char *text;
	int color[] = {255, 255, 255, 255};
	SDL_Color scolor;
	SDL_Surface *text_surface;
	Lua_Image *ptr;

	if (SDL_GetVideoSurface() == NULL)
		return luaL_error(L, "run " NAMESPACE ".init() before generating images");
	if (lua_istable(L, 2)) {
		/* the text is array element 1 */
		lua_pushinteger(L, 1);
		lua_gettable(L, -2);
		if (!lua_isstring(L, -1))
			return luaL_argerror(L, 2, "invalid or missing text in array");
		text = lua_tostring(L, -1); /* stays on the stack */
		/* check for "color" entry */
		lua_getfield(L, -2, "color");
		if (lua_istable(L, -1)) {
			if (!getint(L, &color[0], 1) || !getint(L, &color[1], 2) || !getint(L, &color[2], 3))
				return luaL_argerror(L, 1, "'color' has invalid or missing elements");
			/* the alpha value of the color is optional */
			getint(L, &color[3], 4);
		}
	} else {
		text = luaL_checkstring(L, 2);
	}
	scolor.r = (Uint8)color[0];
	scolor.g = (Uint8)color[1];
	scolor.b = (Uint8)color[2];
	if (!(text_surface = TTF_RenderText_Blended(*font_ptr, text, scolor)))
		return luaL_error(L, "Error rendering text: %s", TTF_GetError());
	ptr = lua_newuserdata(L, sizeof(Lua_Image));
	createTexture(text_surface, ptr, (GLubyte)color[3]);
	SDL_FreeSurface(text_surface);
	luaL_getmetatable(L, "scrupp.image");
	lua_setmetatable(L, -2);
	return 1;
}

static int font_gc(lua_State *L) {
	/* stack(-1): font userdata to free */
	TTF_Font **ptr = checkfont(L);
	TTF_CloseFont(*ptr);
	/* fprintf(stdout, "Freed font.\n"); */
	return 0;
}

static int font_tostring(lua_State *L) {
	lua_pushliteral(L, "Font");
	return 1;
}

static const struct luaL_Reg fontlib [] = {
	{"addFont", Lua_Font_load},
	{NULL, NULL}
};

static const struct luaL_Reg fontlib_m [] = {
	{"__gc", font_gc},
	{"__tostring", font_tostring},
	{"getTextSize", Lua_Font_getTextSize},
	{"getHeight", Lua_Font_getFontHeight},
	{"getLineSkip", Lua_Font_getFontLineSkip},
	{"generateImage", Lua_Font_generateImage},
	{NULL, NULL}
};

int luaopen_font(lua_State *L, const char *parent) {
	/* initialize SDL_ttf */
	if ( TTF_Init() == -1 )
		return luaL_error(L, "Error running TTF_Init: %s", TTF_GetError());
	atexit(TTF_Quit);
	luaL_newmetatable(L, "scrupp.font");
	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);	/* duplicates the metatable */
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, fontlib_m);
	lua_pop(L, 1);	/* pop the metatable */
	luaL_register(L, parent, fontlib);
	return 1;
}


