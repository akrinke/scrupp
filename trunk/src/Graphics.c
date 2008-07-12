/*
** $Id$
** Graphic-related functions and library to use them with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#include <SDL_opengl.h>
#include <SDL_image.h>

#include "Main.h"
#include "Macros.h"
#include "Graphics.h"
#include "physfsrwops.h"

#define checkimage(L) \
	(Lua_Image *)luaL_checkudata(L, 1, "game.image")

typedef struct GLrect {
	GLfloat x1,y1,x2,y2;
} GLrect;

SDL_Surface *screen;

/* calculates the next higher power of two */
static int nextHigherPowerOfTwo(int k) {
	int i;
	k--;
	for (i=1; i<sizeof(int)*8; i=i*2)
		k = k | k >> i;
	return k+1;
}

/* generate an OpenGL texture (wrapped in a Lua_Image) from a SDL_Surface */
int createTexture(SDL_Surface *src, Lua_Image *dest, GLubyte alpha) {
	SDL_Surface *new_surface;
	Uint32 rmask, gmask, bmask, amask;
	GLint nrOfColors;
	GLenum texture_format;
	GLuint texture;
	int old_width, old_height, new_width, new_height;
	old_width = src->w;
	old_height = src->h;
	new_width = nextHigherPowerOfTwo(old_width);
	new_height = nextHigherPowerOfTwo(old_height);
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		rmask = 0xff000000;
		gmask = 0x00ff0000;
		bmask = 0x0000ff00;
		amask = 0x000000ff;
	#else
		rmask = 0x000000ff;
		gmask = 0x0000ff00;
		bmask = 0x00ff0000;
		amask = 0xff000000;
	#endif
	new_surface = SDL_CreateRGBSurface( SDL_SWSURFACE, new_width, new_height, 32,
										rmask, gmask, bmask, amask);
	if ( new_surface == NULL )
		return luaL_error(L, "CreateRGBSurface failed: %s", SDL_GetError());
	SDL_SetAlpha(src, 0, SDL_ALPHA_TRANSPARENT);
	SDL_BlitSurface(src, NULL, new_surface, NULL);
	/* get the number of channels in the SDL surface */
	nrOfColors = new_surface->format->BytesPerPixel;
	if ( nrOfColors == 4 ) {		/* with alpha channel */
		if ( new_surface->format->Rmask == rmask )
			texture_format = GL_RGBA;
		else
			texture_format = GL_BGRA;
	} else if ( nrOfColors == 3 ) {	/* no alpha channel */
		if ( new_surface->format->Rmask == rmask )
			texture_format = GL_RGB;
		else
			texture_format = GL_BGR;
	} else {
		return luaL_error( L, "Image is not truecolor!" );
	}
	/* generate texture object handle */
	glGenTextures( 1, &texture );
	/* bind the texture object */
	glBindTexture( GL_TEXTURE_2D, texture );
	/* set the texture's stretching properties */
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	/* edit the texture's image data using the information from the surface */
	glTexImage2D( 	GL_TEXTURE_2D, 0, nrOfColors, new_width, new_height, 0,
					texture_format, GL_UNSIGNED_BYTE, new_surface->pixels );
	dest->src = new_surface;
	dest->texture = texture;
	dest->w = old_width;
	dest->h = old_height;
	dest->po2width = new_width;
	dest->po2height = new_height;
	/* ratio is needed for blitting the old size of the image, not the new one */
	dest->xratio = (float) old_width / new_width;
	dest->yratio = (float) old_height / new_height;
	dest->alpha = alpha;
	return 0;
}

/* blit an OpenGL texture (wrapped in a Lua_Image) */
int blitTexture(int x, int y, Lua_Image *image, myRect *clip_rect) {
	GLrect texCoords = { 0, 0, image->xratio, image->yratio };
	int width = image->w;
	int height = image->h;
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, image->texture );
	if (clip_rect) {
		texCoords.x1 = (GLfloat) clip_rect->x / image->po2width;
		texCoords.y1 = (GLfloat) clip_rect->y / image->po2height;
		texCoords.x2 = (GLfloat) (clip_rect->x + clip_rect->w) / image->po2width;
		texCoords.y2 = (GLfloat) (clip_rect->y + clip_rect->h) / image->po2height;
		width = clip_rect->w;
		height = clip_rect->h;
	}
	glColor4ub( 255, 255, 255, image->alpha);
	glBegin( GL_QUADS );
		glTexCoord2f( texCoords.x1, texCoords.y1 );
		glVertex3i( x, y, 0 );
		glTexCoord2f( texCoords.x1, texCoords.y2 );
		glVertex3i( x, y+height, 0 );
		glTexCoord2f( texCoords.x2, texCoords.y2 );
		glVertex3i( x+width, y+height, 0 );
		glTexCoord2f( texCoords.x2, texCoords.y1 );
		glVertex3i( x+width, y, 0 );
	glEnd();
	return 0;
}

/* initialize SDL */
static int initSDL (lua_State *L, const char *appName, int width, int height, int bpp, int fullscreen) {
	Uint32 flags;
	flags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_HWSURFACE | SDL_HWACCEL;
	if ( fullscreen )
		flags |= SDL_FULLSCREEN;
	if ( SDL_Init ( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) != 0 )
		luaL_error(L, "Couldn't initialize SDL: %s", SDL_GetError ());
	atexit(SDL_Quit);
	/* set window caption */
	SDL_WM_SetCaption (appName, appName);
	/* enable double buffering */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);
	screen = SDL_SetVideoMode(width, height, bpp, flags);
	if (screen == NULL)
		return luaL_error(L, 	"Couldn't set %dx%dx%d video mode: %s",
								width, height, bpp, SDL_GetError());
	/* set the OpenGL state */
	/* set background color */
	glClearColor( 0, 0, 0, 1);
	/* set line antialiasing */
	glEnable(GL_LINE_SMOOTH);
	/* enable blending */
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	/* enable backface culling */
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glViewport( 0, 0, width, height );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, width, height, 0, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	return 0;
}

static Uint32 getpixel(SDL_Surface *surface, int x, int y) {
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	switch(bpp) {
	case 1:
		return *p;
	case 2:
		return *(Uint16 *)p;
	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
	case 4:
		return *(Uint32 *)p;
	default:
		return 0;	/* shouldn't happen, but avoids warnings */
	}
}

/* Lua functions */

static int Lua_Graphics_init(lua_State *L) {
	const char* appName = luaL_checkstring(L, 1);
	int width = luaL_checkint(L, 2);
	int height = luaL_checkint(L, 3);
	int bpp = luaL_checkint(L, 4);
	int fullscreen;
	luaL_checkany(L, 5);
	fullscreen = lua_toboolean(L, 5);
	initSDL(L, appName, width, height, bpp, fullscreen);
	return 0;
}

static int Lua_Graphics_showCursor(lua_State *L) {
	int result;
	if (lua_isnoneornil(L, 1)) {
		result = SDL_ShowCursor(SDL_QUERY);
		if (result == SDL_ENABLE)
			lua_pushboolean(L, 1);
		else
			lua_pushboolean(L, 0);
	} else {
		luaL_checkany(L, -1);
		result = lua_toboolean(L, 1);
		lua_pushnil(L);
		if (result)
			SDL_ShowCursor(SDL_ENABLE);
		else
			SDL_ShowCursor(SDL_DISABLE);
	}
	return 1;
}

static int Lua_Graphics_getWindowWidth(lua_State *L) {
	lua_pushinteger(L, screen->w);
	return 1;
}

static int Lua_Graphics_getWindowHeight(lua_State *L) {
	lua_pushinteger(L, screen->h);
	return 1;
}

static int Lua_Graphics_getTicks(lua_State *L) {
	lua_pushinteger(L, SDL_GetTicks());
	return 1;
}

static int Lua_Image_load(lua_State *L) {
	const char* filename = luaL_checkstring(L, 1);
	SDL_RWops *temp;
	SDL_Surface *surface;
	Lua_Image *ptr;

	if (SDL_GetVideoSurface() == NULL)
		return luaL_error(L, "Run game.init() before loading images.");
	temp = PHYSFSRWOPS_openRead(filename);
	if (!temp)
		return luaL_error(L, "Error loading file '%s': %s", filename, SDL_GetError());
	surface = IMG_Load_RW(temp, 1);
	if (!surface)
		return luaL_error(L, "Error loading file '%s': %s", filename, IMG_GetError());
	ptr = lua_newuserdata(L, sizeof(Lua_Image));
	createTexture(surface, ptr, 255);
	SDL_FreeSurface(surface);
	luaL_getmetatable(L, "game.image");
	lua_setmetatable(L, -2);
	return 1;
}

static int Lua_Image_getWidth(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushinteger(L, image->w);
	return 1;
}

static int Lua_Image_getHeight(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushinteger(L, image->h);
	return 1;
}

static int Lua_Image_getSize(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushinteger(L, image->w);
	lua_pushinteger(L, image->h);
	return 2;
}

static int Lua_Image_isTransparent(lua_State *L) {
	Lua_Image *image = checkimage(L);
	int x = luaL_checkint(L, 2);
	int y = luaL_checkint(L, 3);
	Uint32 pixel;
	Uint8 r,g,b,a;
	luaL_argcheck(L, (x>=0) && (x<image->w), 2, "out of image dimension");
	luaL_argcheck(L, (y>=0) && (y<image->h), 3, "out of image dimension");
	pixel = getpixel(image->src, x, y);
	SDL_GetRGBA(pixel, image->src->format, &r, &g, &b, &a);
	lua_pushboolean(L, a != 0xff);
	return 1;
}

static int Lua_Image_setAlpha(lua_State *L) {
	Lua_Image *image = checkimage(L);
	int alpha = luaL_checkint(L, 2);
	luaL_argcheck(L, (alpha>=0) && (alpha<=255), 2, "0 <= alpha <= 255");
	image->alpha = (GLubyte) alpha;
	return 0;
}

static int Lua_Image_getAlpha(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushinteger(L, image->alpha);
	return 1;
}

static int Lua_Image_render(lua_State *L) {
	Lua_Image *image = checkimage(L);
	int x, y;
	myRect clip_rect;
	myRect *pclip_rect = NULL;

	if (lua_istable(L, 2)) {
		/* x and y are array element 1 and 2 */
		if (!getint(L, &x, 1) || !getint(L, &y, 2))
			return luaL_argerror(L, 2, "invalid x or y component in array");
		/* check for "rect"-entry */
		lua_getfield(L, -1, "rect");
		if (lua_istable(L, -1)) {
			if (!getint(L, &clip_rect.x, 1) || !getint(L, &clip_rect.y, 2) ||
				!getint(L, &clip_rect.w, 3) || !getint(L, &clip_rect.h, 4))
				return luaL_argerror(L, 2, "'rect' has invalid or missing elements");
			pclip_rect = &clip_rect;
		} else if (!lua_isnil(L, -1))
			return luaL_argerror(L, 2, "'rect' should be an array and nothing else");
		/* lua_pop(L, 1); */ /* nescessary if more arguments are implemented */
	} else {
		x = luaL_checkint(L, 2);
		y = luaL_checkint(L, 3);
	}
	blitTexture(x, y, image, pclip_rect);
	return 0;
}

static int image_gc(lua_State *L) {
	Lua_Image *image = checkimage(L);
	glDeleteTextures( 1, &image->texture );
	SDL_FreeSurface(image->src);
	/* fprintf(stdout, "Freed image.\n"); */
	return 0;
}

static int image_tostring(lua_State *L) {
	lua_pushliteral(L, "Image");
	return 1;
}

static int Lua_Graphics_draw(lua_State *L){
	int i;
	int n;
	int *coords;
	int color[] = {255, 255, 255, 255};
	GLfloat size = 1.0f;
	int connect = 0;
	int fill = 0;
	int pixelList = 0;
	int antialias = 0;

	luaL_checktype(L, 1, LUA_TTABLE);
	/* get number of elements in the array part of the table */
	n = luaL_getn(L, 1);	/* alternative: lua_objlen(L, 1); */
	luaL_argcheck(L, n % 2 == 0, 1, "even number of x- and y-coordinates needed");
	/* collect all coordinates in array */
	coords = (int*)malloc(n*sizeof(int));
	for (i=0; i<n; i++) {
		if (!getint(L, &coords[i], i+1)) {
			free(coords);
			return luaL_argerror(L, 1, "invalid x or y component in array");
		}
	}
	/* check for "color"-entry */
	lua_getfield(L, -1, "color");
	if (lua_istable(L, -1)) {
		if (!getint(L, &color[0], 1) || !getint(L, &color[1], 2) || !getint(L, &color[2], 3)) {
				free(coords);
				return luaL_argerror(L, 1, "'color' has invalid or missing elements");
		}
		/* the alpha value of the color is optional */
		getint(L, &color[3], 4);
	} else if (!lua_isnil(L, -1)) {
		free(coords);
		return luaL_argerror(L, 1, "'color' should be an array and nothing else");
	}
	lua_pop(L, 1);
	/* check for "size"-entry */
	lua_getfield(L, -1, "size");
	if (lua_isnumber(L, -1))
		size = (float)lua_tonumber(L, -1);
	else if (!lua_isnil(L, -1)) {
		free(coords);
		return luaL_argerror(L, 1, "'size' should be a number and nothing else");
	}
	lua_pop(L, 1);
	/* check for "connect"-entry */
	lua_getfield(L, -1, "connect");
	connect = lua_toboolean(L, -1);
	lua_pop(L, 1);
	/* check for "fill"-entry */
	lua_getfield(L, -1, "fill");
	fill = lua_toboolean(L, -1);
	lua_pop(L, 1);
	/* check for "antialiasing"-entry */
	lua_getfield(L, -1, "antialiasing");
	antialias = lua_toboolean(L, -1);
	lua_pop(L, 1);
	/* check for "pixellist"-entry */
	lua_getfield(L, -1, "pixellist");
	pixelList = lua_toboolean(L, -1);
	lua_pop(L, 1);

	if (!connect && !fill && !pixelList && (n % 4 != 0) && (n > 2)) {
		free(coords);
		luaL_argerror(L, 1, "unconnected lines need an even number of coordinate-pairs");
	}

	glColor4ub((GLubyte)color[0], (GLubyte)color[1], (GLubyte)color[2], (GLubyte)color[3]);
	glPointSize(size);
	glLineWidth(size);

	if (antialias) {
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POLYGON_SMOOTH); /* not well supported */
	} else {
		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_LINE_SMOOTH);
		glDisable(GL_POLYGON_SMOOTH); /* not well supported */
	}

	glDisable(GL_TEXTURE_2D);
	if (n == 2 || pixelList)
		glBegin(GL_POINTS);
	else if (fill) {
		if (n == 4)
			glBegin(GL_LINE_STRIP);
		else if (n == 6)
			glBegin(GL_TRIANGLES);
		else if (n == 8)
			glBegin(GL_QUADS);
		else if (n > 8)
			glBegin(GL_POLYGON);
	} else {
		if (connect)
			glBegin(GL_LINE_STRIP);
		else
			glBegin(GL_LINES);
	}

	for (i=n-2; i>=0; i=i-2)
		glVertex3i(coords[i], coords[i+1], 0);
	glEnd();

	free(coords);
	return 0;
}


static const struct luaL_Reg graphicslib [] = {
	{"init", 				Lua_Graphics_init},
	{"getWindowWidth", 		Lua_Graphics_getWindowWidth},
	{"getWindowHeight", 	Lua_Graphics_getWindowHeight},
	{"showCursor", 			Lua_Graphics_showCursor},
	{"getTicks", 			Lua_Graphics_getTicks},
	{"addImage", 			Lua_Image_load},
	{"draw", 				Lua_Graphics_draw},
	{NULL, NULL}
};

static const struct luaL_Reg imagelib_m [] = {
	{"__gc", 				image_gc},
	{"__tostring", 			image_tostring},
	{"getWidth", 			Lua_Image_getWidth},
	{"getHeight", 			Lua_Image_getHeight},
	{"getSize", 			Lua_Image_getSize},
	{"isTransparent", 		Lua_Image_isTransparent},
	{"setAlpha", 			Lua_Image_setAlpha},
	{"getAlpha", 			Lua_Image_getAlpha},
	{"render", 				Lua_Image_render},
	{NULL, NULL}
};

int luaopen_graphics(lua_State *L, const char *parent) {
	luaL_newmetatable(L, "game.image");
	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);	/* duplicates the metatable */
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, imagelib_m);
	lua_pop(L, 1);	/* pop the metatable */
	luaL_register(L, parent, graphicslib);
	return 1;
}
