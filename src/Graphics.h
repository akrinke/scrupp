/*
** $Id$
** Graphic-related functions and library to use them with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <SDL_opengl.h>

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	#define RMASK 0xFF000000
	#define GMASK 0x00FF0000
	#define BMASK 0x0000FF00
	#define AMASK 0x000000FF
#else
	#define RMASK 0x000000FF
	#define GMASK 0x0000FF00
	#define BMASK 0x00FF0000
	#define AMASK 0xFF000000
#endif

typedef struct Lua_Image {
	/* OpenGL texture ids */
	GLuint *textures;
	GLenum texture_format;
	/* surface with the source image; used for getpixel */
	SDL_Surface *src;
	/* width and height of the original image */
	int w;
	int h;
	/* width and height of the texture after scaling it up to a power of two */
	int po2_width;
	int po2_height;
	/* if the texture needs to be subdivided in tiles, this is the size of one */
	GLint tile_width;
	GLint tile_height;
	/* number of tiles in x- and y-direction */
	GLubyte x_tiles;
	GLubyte y_tiles;
	/* alpha value of the whole texture */
	GLubyte alpha;
	/* all textures are stored in a double-linked-list for easy reloading */
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
int createTexture(lua_State *L, SDL_Surface *src, Lua_Image *dest, GLubyte alpha);

int luaopen_graphics(lua_State *L, const char *parent);

#endif /* __GRAPHICS_H__ */
