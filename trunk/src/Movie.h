/*
** $Id$
** Library for MPEG support
** See Copyright Notice in COPYRIGHT
*/

#ifndef __MOVIE_H__
#define __MOVIE_H__

#include <smpeg/smpeg.h>
#include <SDL_opengl.h>

typedef struct Lua_Movie {
	GLuint texture;
	SMPEG_Info *info;
	SMPEG *movie;
	SDL_Surface *surface;
	GLubyte alpha;
} Lua_Movie;

int luaopen_movie(lua_State *L, const char *parent);

#endif /* __MOVIE_H__ */
