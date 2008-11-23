/*
** $Id$
** Graphic-related functions and library to use them with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <SDL_opengl.h>
typedef struct Lua_Image {
	GLuint texture;	/* OpenGL texture id */
	GLenum texture_format;
	SDL_Surface *src; /* surface with the source image; used for getpixel */
	int w;			/* width of the texture */
	int h;			/* height of the texture */
	int po2width;	/* width of the texture after scaling it up to a power of two */
	int po2height;	/* height of the texture after scaling it up to a power of two */
	float xratio;
	float yratio;
	GLubyte alpha;	/* alpha value of the whole texture */
	struct Lua_Image *next;
	struct Lua_Image *prev;
} Lua_Image;

typedef struct myRect {
	int x, y, w, h;
} myRect;

typedef struct GLrect {
	GLfloat x1, y1, x2, y2;
} GLrect;

unsigned int nextHigherPowerOfTwo(unsigned int k);
int sendTextureToCard(Lua_Image *img);
int createTexture(SDL_Surface *src, Lua_Image *dest, GLubyte alpha);

int luaopen_graphics(lua_State *L, const char *parent);

#endif /* __GRAPHICS_H__ */
