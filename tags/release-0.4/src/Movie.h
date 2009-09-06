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
	int po2_width;
	int po2_height;
	SMPEG_Info *info;
	SMPEG *movie;
	SDL_Surface *surface;
	GLubyte alpha;
	int music_hooked;
	int changed;
	/* all movies are stored in a double-linked-list for easy reloading */
	struct Lua_Movie *next;
	struct Lua_Movie *prev;
} Lua_Movie;

/* first node of the double-linked list 
   containing all loaded movies */
extern Lua_Movie *first_movie;

/* mutex for all loaded movies
   It is activated during a call to scrupp.init() */
extern SDL_mutex *movie_mutex;

int luaopen_movie(lua_State *L, const char *parent);

#endif /* __MOVIE_H__ */
