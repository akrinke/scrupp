/*
** $Id$
** Graphic-related functions and library to use them with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#include <SDL_opengl.h>
#include <SDL_image.h>
#include <cairo.h>
#include <math.h>
#include <limits.h>

#include "lua-oocairo/oocairo.h"

#include "Main.h"
#include "Macros.h"
#include "Graphics.h"
#include "Movie.h"
#include "physfsrwops.h"
#include "IMG_savepng.h"

#define checkimage(L) \
	(Lua_Image *)luaL_checkudata(L, 1, "scrupp.image")

SDL_Surface *screen;

/* Initial value for maximum texture size.
 * OpenGL standard defines 64 as the lower limit.
 * Has to be power of two.
 */
int max_texture_size = 64;

/* glClearColor, stored as byte to avoid rounding errors if returned */
GLubyte r_clear = 0;
GLubyte g_clear = 0;
GLubyte b_clear = 0;

Lua_Image *first_image = NULL;

/* calculates the next higher power of two */
unsigned int nextHigherPowerOfTwo(unsigned int k) {
	unsigned int i;
	k--;
	for (i=1; i<sizeof(int)*8; i=i*2)
		k = k | k >> i;
	return k+1;
}

static int sendTextureToCard(lua_State *L, Lua_Image *img) {
	SDL_Surface *tile_surface;
	SDL_Rect src_rect;
	GLuint *textures;
	int x, y;

	if ((img->x_tiles == 1) && (img->y_tiles == 1)) {
		/* generate texture object */
		textures = (GLuint *)malloc(sizeof(GLuint));
		glGenTextures(1, textures);
		glBindTexture(GL_TEXTURE_2D, *textures);
		/* set the texture's stretching properties */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// the texture does not wrap over at the edges (no repeat)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		/* edit the texture's image data using the information from the surface */
		glTexImage2D(GL_TEXTURE_2D, 0, img->src->format->BytesPerPixel,
		             img->po2_width, img->po2_height, 0,
		             img->texture_format, GL_UNSIGNED_BYTE, img->src->pixels );
	} else {
		src_rect.w = img->tile_width;
		src_rect.h = img->tile_height;
		
		tile_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, src_rect.w, src_rect.h, 32,
		                                    RMASK, GMASK, BMASK, AMASK);
		if (tile_surface == NULL)
			return luaL_error(L, "CreateRGBSurface failed: %s", SDL_GetError());
		
		SDL_SetAlpha(img->src, 0, SDL_ALPHA_TRANSPARENT);
		textures = (GLuint *)malloc(sizeof(GLuint) * img->x_tiles * img->y_tiles);
		glGenTextures(img->x_tiles * img->y_tiles, textures);
		
		for (y=0; y<img->y_tiles; y++) {
			src_rect.y = y*img->tile_height;
			for (x=0; x<img->x_tiles; x++) {
				src_rect.x = x*img->tile_width;
				SDL_BlitSurface(img->src, &src_rect, tile_surface, NULL);
				glBindTexture(GL_TEXTURE_2D, textures[x+y*img->x_tiles]);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, img->src->format->BytesPerPixel,
				             src_rect.w, src_rect.h, 0,
				             img->texture_format, GL_UNSIGNED_BYTE, tile_surface->pixels );
			}
		}
		SDL_FreeSurface(tile_surface);
	}

	img->textures = textures;
	return 0;
}

/* generate an OpenGL texture (wrapped in a Lua_Image) from a SDL_Surface */
int createTexture(lua_State *L, SDL_Surface *src, Lua_Image *dest, GLubyte alpha) {
	SDL_Surface *new_surface;
	GLint nrOfColors;
	GLenum texture_format;
	int old_width, old_height, new_width, new_height;
	GLint dummy_width;
	int tile_width, tile_height;
	/* number of tiles in x and y direction */
	char x_tiles, y_tiles;
	/* orignal width and height of the image */
	old_width = src->w;
	old_height = src->h;
	/* OpenGL only supports textures with a power of two as width and height */
	new_width = nextHigherPowerOfTwo(old_width);
	new_height = nextHigherPowerOfTwo(old_height);
	/* images that are bigger than what is supported by OpenGL need to be tiled */
	tile_width = new_width;
	tile_height = new_height;

	new_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, new_width, new_height, 32,
	                                   RMASK, GMASK, BMASK, AMASK);
	if ( new_surface == NULL )
		return luaL_error(L, "CreateRGBSurface failed: %s", SDL_GetError());

	SDL_SetAlpha(src, 0, SDL_ALPHA_TRANSPARENT);
	SDL_BlitSurface(src, NULL, new_surface, NULL);

	/* get the number of channels in the SDL surface */
	nrOfColors = new_surface->format->BytesPerPixel;
	if ( nrOfColors == 4 ) {		/* with alpha channel */
		if ( new_surface->format->Rmask == RMASK ) {
			texture_format = GL_RGBA;
		} else {
			texture_format = GL_BGRA;
		}
	} else if ( nrOfColors == 3 ) {	/* no alpha channel */
		if (new_surface->format->Rmask == RMASK) {
			texture_format = GL_RGB;
		} else {
			texture_format = GL_BGR;
		}
	} else {
		return luaL_error(L, "Image is not truecolor!");
	}
	
	x_tiles = (char) ceil((double)old_width / max_texture_size);
	y_tiles = (char) ceil((double)old_height / max_texture_size);
	
	dest->src = new_surface;
	dest->texture_format = texture_format;
	dest->w = old_width;
	dest->h = old_height;
	dest->po2_width = new_width;
	dest->po2_height = new_height;
	dest->tile_width = tile_width;
	dest->tile_height = tile_height;
	dest->x_tiles = x_tiles;
	dest->y_tiles = y_tiles;
	dest->alpha = alpha;
	dest->color = NULL;
	dest->x = 0.0;
	dest->y = 0.0;
	dest->center_x = 0.0;
	dest->center_y = 0.0;
	dest->scale_x = 1.0;
	dest->scale_y = 1.0;
	dest->rotation = 0.0;
	dest->rect = NULL;
	dest->next = NULL;
	dest->prev = NULL;
	
	sendTextureToCard(L, dest);

	if (first_image == NULL) {
		first_image = dest;
	} else {
		first_image->prev = dest;
		dest->next = first_image;
		first_image = dest;
	}
	return 0;
}

/* initialize SDL */
static int initSDL (lua_State *L, const char *appName, int width, int height, int bpp, int fullscreen, int resizable) {
	Uint32 flags;
	Lua_Image *image_node;
	Lua_Movie *movie_node;
	int dummy_size = 1;
	SDL_Surface *movie_po2_surface;
	flags = SDL_OPENGL;
	
	SDL_LockMutex(movie_mutex);
	
	if (fullscreen) {
		flags |= SDL_FULLSCREEN;
	} else if (resizable) {
		flags |= SDL_RESIZABLE;
	}
	if (screen != NULL) {
		/* delete all OpenGL textures */
		image_node = first_image;
		while (image_node != NULL) {
			glDeleteTextures(image_node->x_tiles*image_node->y_tiles, image_node->textures);
			image_node = image_node->next;
		}
		movie_node = first_movie;
		while (movie_node != NULL) {
			glDeleteTextures(1, &movie_node->texture);
			movie_node = movie_node->next;
		}
	}
	/* set window caption */
	SDL_WM_SetCaption (appName, appName);
	/* center new window */
	SDL_putenv("SDL_VIDEO_CENTERED=1");
	/* enable double buffering */
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  5);
	screen = SDL_SetVideoMode(width, height, bpp, flags);
	if (screen == NULL)
		return luaL_error(L, "Couldn't set %dx%dx%d video mode: %s",
		                     width, height, bpp, SDL_GetError());
	/* set the OpenGL state */
	/* set background color */
	glClearColor((GLfloat)r_clear/255.0f, (GLfloat)g_clear/255.0f, (GLfloat)b_clear/255.0f, 1.0f);
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

	/* determine the maximum texture size */
	while (dummy_size != 0 && max_texture_size <= INT_MAX/2) {
		max_texture_size *= 2;
		glTexImage2D(GL_PROXY_TEXTURE_2D, 0, 4, 
		             max_texture_size, max_texture_size, 0,
		             GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glGetTexLevelParameteriv(GL_PROXY_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &dummy_size);
	}
	if (dummy_size == 0)
		max_texture_size /= 2;

	/* reloading all images */
	image_node = first_image;
	while (image_node != NULL) {
		sendTextureToCard(L, image_node);
		image_node = image_node->next;
	}
	/* reloading all movies */
	movie_node = first_movie;
	while (movie_node != NULL) {
		movie_po2_surface = SDL_CreateRGBSurface(SDL_SWSURFACE,
			movie_node->po2_width, movie_node->po2_height, 32,
			RMASK, GMASK, BMASK, AMASK);
		glGenTextures(1, &movie_node->texture);
		glBindTexture(GL_TEXTURE_2D, movie_node->texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
		             movie_node->po2_width, movie_node->po2_height, 0, 
		             GL_RGBA, GL_UNSIGNED_BYTE, movie_po2_surface->pixels );
		SDL_FreeSurface(movie_po2_surface);
		movie_node = movie_node->next;
	}
	SDL_UnlockMutex(movie_mutex);
	return 0;
}

static Uint32 getpixel(SDL_Surface *surface, int x, int y) {
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
	switch (bpp) {
	case 1:
		return *p;
	case 2:
		return *(Uint16 *)p;
	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
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

static int Lua_Graphics_getFullscreenModes(lua_State *L) {
	SDL_Rect **modes;
	int i;

	modes = SDL_ListModes(NULL, SDL_OPENGL|SDL_FULLSCREEN);
	if (modes == (SDL_Rect **)0)
		lua_pushnil(L);
	else if (modes == (SDL_Rect **)-1)
		lua_pushboolean(L, 1);
	else {
		lua_newtable(L);
		for (i=0; modes[i]; ++i) {
			lua_createtable(L, 2, 0);
			lua_pushinteger(L, modes[i]->w);
			lua_rawseti(L, -2, 1);
			lua_pushinteger(L, modes[i]->h);
			lua_rawseti(L, -2, 2);
			lua_rawseti(L, -2, i+1);
		}
	}
	return 1;
}

static int Lua_Graphics_init(lua_State *L) {
	const char* appName = luaL_checkstring(L, 1);
	int width = luaL_checkint(L, 2);
	int height = luaL_checkint(L, 3);
	int bpp = luaL_checkint(L, 4);
	int fullscreen;
	int resizable;
	luaL_checkany(L, 5);
	fullscreen = lua_toboolean(L, 5);
	/* lua_toboolean returns 0 when called with a non-valid index
	   this way resizable becomes optional */
	resizable = lua_toboolean(L, 6);
	initSDL(L, appName, width, height, bpp, fullscreen, resizable);
	return 0;
}

static int Lua_Graphics_showCursor(lua_State *L) {
	int result;
	if (lua_isnone(L, 1)) {
		result = SDL_ShowCursor(SDL_QUERY);
		if (result == SDL_ENABLE) {
			lua_pushboolean(L, 1);
		} else {
			lua_pushboolean(L, 0);
		}
		return 1;
	} else {
		result = lua_toboolean(L, 1);
		lua_pushnil(L);
		if (result) {
			SDL_ShowCursor(SDL_ENABLE);
		} else {
			SDL_ShowCursor(SDL_DISABLE);
		}
		return 0;
	}
}

static int Lua_Graphics_setBackgroundColor(lua_State *L) {
	if (lua_istable(L, 1)) {
		lua_pushinteger(L, 1);
		lua_gettable(L, 1);
		lua_pushinteger(L, 2);
		lua_gettable(L, 1);
		lua_pushinteger(L, 3);
		lua_gettable(L, 1);
		lua_remove(L, 1);
	}
	r_clear = (GLubyte)luaL_checkint(L, 1);
	g_clear = (GLubyte)luaL_checkint(L, 2);
	b_clear = (GLubyte)luaL_checkint(L, 3);
	glClearColor((GLfloat)r_clear/255.0f, (GLfloat)g_clear/255.0f, (GLfloat)b_clear/255.0f, 1.0f);
	return 0;
}

static int Lua_Graphics_getBackgroundColor(lua_State *L) {
	lua_pushinteger(L, r_clear);
	lua_pushinteger(L, g_clear);
	lua_pushinteger(L, b_clear);
	return 3;
}

static int Lua_Graphics_getWindowWidth(lua_State *L) {
	lua_pushinteger(L, screen->w);
	return 1;
}

static int Lua_Graphics_getWindowHeight(lua_State *L) {
	lua_pushinteger(L, screen->h);
	return 1;
}

static int Lua_Graphics_getWindowSize(lua_State *L) {
	lua_pushinteger(L, screen->w);
	lua_pushinteger(L, screen->h);
	return 2;
}

static int Lua_Graphics_getTicks(lua_State *L) {
	lua_pushinteger(L, SDL_GetTicks());
	return 1;
}

static int Lua_Graphics_translateView(lua_State *L) {
	GLdouble translateX = (GLdouble)luaL_checknumber(L, 1);
	GLdouble translateY = (GLdouble)luaL_checknumber(L, 2);
	glTranslated(translateX, translateY, 0);
	return 0;
}

static int Lua_Graphics_scaleView(lua_State *L) {
	GLdouble scaleX = (GLdouble)luaL_checknumber(L, 1);
	GLdouble scaleY = (GLdouble)luaL_checknumber(L, 2);
	glScaled(scaleX, scaleY, 0);
	return 0;
}

static int Lua_Graphics_rotateView(lua_State *L) {
	GLdouble rotate = (GLdouble)luaL_checknumber(L, 1);
	glRotated(rotate, 0, 0, 1);
	return 0;
}

static int Lua_Graphics_saveView(lua_State *L) {
	glPushMatrix();
	if (glGetError() == GL_STACK_OVERFLOW)
		return luaL_error(L, "No more free slots to save the view.");
	return 0;
}

static int Lua_Graphics_restoreView(lua_State *L) {
	glPopMatrix();
	if (glGetError() == GL_STACK_UNDERFLOW)
		return luaL_error(L, "No saved view was found.");
	return 0;
}

static int Lua_Graphics_resetView(lua_State *L) {
	UNUSED(L);
	glLoadIdentity();
	return 0;
}

static int Lua_Image_load(lua_State *L) {
	const char* filename = luaL_checkstring(L, 1);
	SDL_RWops *temp;
	SDL_Surface *surface;
	Lua_Image *ptr;

	if (SDL_GetVideoSurface() == NULL)
		return luaL_error(L, "Run " NAMESPACE ".init() before loading images.");
	temp = PHYSFSRWOPS_openRead(filename);
	if (!temp)
		return luaL_error(L, "Error loading file '%s': %s", filename, SDL_GetError());
	surface = IMG_Load_RW(temp, 1);
	if (surface == NULL)
		return luaL_error(L, "Error loading file '%s': %s", filename, IMG_GetError());
	ptr = lua_newuserdata(L, sizeof(Lua_Image));
	createTexture(L, surface, ptr, 255);
	SDL_FreeSurface(surface);
	luaL_getmetatable(L, "scrupp.image");
	lua_setmetatable(L, -2);
	return 1;
}

static int Lua_Image_loadFromString(lua_State *L) {
	size_t len;
	const char* string = luaL_checklstring(L, 1, &len);
	SDL_RWops *rw;
	SDL_Surface *surface;
	Lua_Image *ptr;

	if (SDL_GetVideoSurface() == NULL)
		return luaL_error(L, "Run " NAMESPACE ".init() before loading images.");
	rw = SDL_RWFromConstMem(string, len);
	if (rw == NULL)
		return luaL_error(L, "Error reading string.");
	surface = IMG_Load_RW(rw, 1);
	if (surface == NULL)
		return luaL_error(L, "Error loading image from string: %s", IMG_GetError());
	ptr = lua_newuserdata(L, sizeof(Lua_Image));
	createTexture(L, surface, ptr, 255);
	SDL_FreeSurface(surface);
	luaL_getmetatable(L, "scrupp.image");
	lua_setmetatable(L, -2);
	return 1;
}

static int Lua_Image_loadFromCairoSurface(lua_State *L) {
	/* the name of the metatable is defined in the sources of Lua-OOCairo (oocairo.c) */
	cairo_surface_t **cairo_surface = (cairo_surface_t **)luaL_checkudata(L, 1, "6d31a064-6711-11dd-bdd8-00e081225ce5");
	unsigned char *cairo_data;
	SDL_Surface *sdl_surface;
	Lua_Image *ptr;	
	int width = cairo_image_surface_get_width(*cairo_surface);
	int height = cairo_image_surface_get_height(*cairo_surface);
	int stride = cairo_image_surface_get_stride(*cairo_surface);
	Uint32 rmask = 0x00FF0000;
	Uint32 gmask = 0x0000FF00;
	Uint32 bmask = 0x000000FF;
	Uint32 amask = 0xFF000000;
	
	if (SDL_GetVideoSurface() == NULL) {
		return luaL_error(L, "Run " NAMESPACE ".init() before loading images.");
	}

	if (cairo_image_surface_get_format(*cairo_surface) == CAIRO_FORMAT_RGB24) {
		amask = 0x00000000;
	}

	cairo_data = cairo_image_surface_get_data(*cairo_surface);	
	if (cairo_data == NULL) {
		return luaL_error(L, "Cairo surface is no image surface!");
	}
	/* printf("width: %d\theight: %d\tstride: %d\n", width, height, stride); */

	sdl_surface = SDL_CreateRGBSurfaceFrom((void *) cairo_data, width, height, 32, stride,
	                                       rmask, gmask, bmask, amask);
	if (sdl_surface == NULL) {
		return luaL_error(L, "Error creating SDL surface: %s", SDL_GetError());
	}
	ptr = lua_newuserdata(L, sizeof(Lua_Image));
	createTexture(L, sdl_surface, ptr, 255);
	SDL_FreeSurface(sdl_surface);
	luaL_getmetatable(L, "scrupp.image");
	lua_setmetatable(L, -2);
	return 1;
}

static int Lua_Cairo_renderSurface(lua_State *L) {
	/* the name of the metatable is defined in the sources of Lua-OOCairo (oocairo.c) */
	cairo_surface_t **cairo_surface = (cairo_surface_t **)luaL_checkudata(L, 1, "6d31a064-6711-11dd-bdd8-00e081225ce5");
	int x = luaL_checkint(L, 2);
	int y = luaL_checkint(L, 3);
	cairo_format_t cairo_format = cairo_image_surface_get_format(*cairo_surface);
	GLenum texture_format;
	unsigned char *cairo_data;
	int width = cairo_image_surface_get_width(*cairo_surface);
	int height = cairo_image_surface_get_height(*cairo_surface);
	int stride = cairo_image_surface_get_stride(*cairo_surface);
	int po2_width = nextHigherPowerOfTwo(width);
	int po2_height = nextHigherPowerOfTwo(height);
	int x_tiles = (int) ceil((double) width / max_texture_size);
	int y_tiles = (int) ceil((double) height / max_texture_size);
	GLuint *textures;
	GLrect tex_coords = {0, 0, (GLfloat) width / po2_width, (GLfloat) height / po2_height};
		
	if (SDL_GetVideoSurface() == NULL) {
		return luaL_error(L, "Run " NAMESPACE ".init() before loading images.");
	}
	
	if (stride != 4*width) {
		return luaL_error(L, "Unexpected stride value for surface.");
	}

	if (cairo_format == CAIRO_FORMAT_ARGB32) {
		texture_format = GL_RGBA;
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	} else if (cairo_format == CAIRO_FORMAT_RGB24) {
		texture_format = GL_RGB;
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	} else {
		return luaL_error(L, "Cairo surface has invalid format (only RGB24 and ARGB32 are supported)!");
	}
	
	cairo_surface_flush(*cairo_surface);
	cairo_data = cairo_image_surface_get_data(*cairo_surface);
	if (cairo_data == NULL) {
		return luaL_error(L, "Cairo surface is no image surface!");
	}
	
	/* generate texture object(s) */
	textures = (GLuint *)malloc(sizeof(GLuint) * x_tiles * y_tiles);
	glGenTextures(x_tiles * y_tiles, textures);
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, 0);
	
	if (x_tiles == 1 && y_tiles == 1) {
		glBindTexture(GL_TEXTURE_2D, *textures);
		/* set the texture's stretching properties */
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// the texture does not wrap over at the edges (no repeat)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, texture_format,
		             po2_width, po2_height, 0,
		             texture_format, GL_UNSIGNED_BYTE, NULL);
		/* TODO: Consider endianness */
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
		                width, height, GL_BGRA, GL_UNSIGNED_BYTE, cairo_data);
		glBegin(GL_QUADS);
			glTexCoord2f(tex_coords.x1, tex_coords.y1);
			glVertex2i(0, 0);
			glTexCoord2f(tex_coords.x1, tex_coords.y2);
			glVertex2i(0, height);
			glTexCoord2f(tex_coords.x2, tex_coords.y2);
			glVertex2i(width, height);
			glTexCoord2f(tex_coords.x2, tex_coords.y1);
			glVertex2i(width, 0);
		glEnd();

	} else {
		/* TODO */
		glPopMatrix();
		glDeleteTextures(x_tiles * y_tiles, textures);
		free(textures);
		return luaL_error(L, "Cairo surface is too large for your graphics hardware.");
	}
	glPopMatrix();
	glDeleteTextures(x_tiles * y_tiles, textures);
	free(textures);
	return 0;
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
	image->alpha = (GLubyte)luaL_checkint(L, 2);
	return 0;
}

static int Lua_Image_getAlpha(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushinteger(L, image->alpha);
	return 1;
}

static int Lua_Image_setColor(lua_State *L) {
	Lua_Image *image = checkimage(L);
	GLubyte r, g, b;
	if (lua_istable(L, 2)) {
		lua_pushinteger(L, 1);
		lua_gettable(L, 2);
		lua_pushinteger(L, 2);
		lua_gettable(L, 2);
		lua_pushinteger(L, 3);
		lua_gettable(L, 2);
		lua_remove(L, 2);
	}
	r = (GLubyte)luaL_checkint(L, 2);
	g = (GLubyte)luaL_checkint(L, 3);
	b = (GLubyte)luaL_checkint(L, 4);
	if (image->color == NULL) {
		image->color = (GLubyte *)malloc(3*sizeof(GLubyte));
	}
	image->color[0] = r;
	image->color[1] = g;
	image->color[2] = b;
	return 0;
}

static int Lua_Image_getColor(lua_State *L) {
	Lua_Image *image = checkimage(L);
	if (image->color == NULL) {
		lua_pushinteger(L, 255);
		lua_pushinteger(L, 255);
		lua_pushinteger(L, 255);
	} else {
		lua_pushinteger(L, image->color[0]);
		lua_pushinteger(L, image->color[1]);
		lua_pushinteger(L, image->color[2]);
	}
	return 3;
}

static int Lua_Image_clearColor(lua_State *L) {
	Lua_Image *image = checkimage(L);
	/* if image->color is NULL, no operation is performed */
	free(image->color);
	image->color = NULL;
	return 0;
}

static int Lua_Image_setX(lua_State *L) {
	Lua_Image *image = checkimage(L);
	image->x = (GLdouble)luaL_checknumber(L, 2);
	return 0;
}

static int Lua_Image_getX(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushnumber(L, image->x);
	return 1;
}

static int Lua_Image_setY(lua_State *L) {
	Lua_Image *image = checkimage(L);
	image->y = (GLdouble)luaL_checknumber(L, 2);
	return 0;
}

static int Lua_Image_getY(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushnumber(L, image->y);
	return 1;
}

static int Lua_Image_setPosition(lua_State *L) {
	Lua_Image *image = checkimage(L);
	image->x = (GLdouble)luaL_checknumber(L, 2);
	image->y = (GLdouble)luaL_checknumber(L, 3);
	return 0;
}

static int Lua_Image_getPosition(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushnumber(L, image->x);
	lua_pushnumber(L, image->y);
	return 2;
}

static int Lua_Image_setCenterX(lua_State *L) {
	Lua_Image *image = checkimage(L);
	image->center_x = (GLdouble)luaL_checknumber(L, 2);
	return 0;
}

static int Lua_Image_getCenterX(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushnumber(L, image->center_x);
	return 1;
}

static int Lua_Image_setCenterY(lua_State *L) {
	Lua_Image *image = checkimage(L);
	image->center_y = (GLdouble)luaL_checknumber(L, 2);
	return 0;
}

static int Lua_Image_getCenterY(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushnumber(L, image->center_y);
	return 1;
}

static int Lua_Image_setCenter(lua_State *L) {
	Lua_Image *image = checkimage(L);
	image->center_x = (GLdouble)luaL_checknumber(L, 2);
	image->center_y = (GLdouble)luaL_checknumber(L, 3);
	return 0;
}

static int Lua_Image_getCenter(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushnumber(L, image->center_x);
	lua_pushnumber(L, image->center_y);
	return 2;
}

static int Lua_Image_setScaleX(lua_State *L) {
	Lua_Image *image = checkimage(L);
	image->scale_x = (GLdouble)luaL_checknumber(L, 2);
	return 0;
}

static int Lua_Image_getScaleX(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushnumber(L, image->scale_x);
	return 1;
}

static int Lua_Image_setScaleY(lua_State *L) {
	Lua_Image *image = checkimage(L);
	image->scale_y = (GLdouble)luaL_checknumber(L, 2);
	return 0;
}

static int Lua_Image_getScaleY(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushnumber(L, image->scale_y);
	return 1;
}

static int Lua_Image_setScale(lua_State *L) {
	Lua_Image *image = checkimage(L);
	image->scale_x = (GLdouble)luaL_checknumber(L, 2);
	image->scale_y = (GLdouble)luaL_checknumber(L, 3);
	return 0;
}

static int Lua_Image_getScale(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushnumber(L, image->scale_x);
	lua_pushnumber(L, image->scale_y);
	return 2;
}

static int Lua_Image_setRotation(lua_State *L) {
	Lua_Image *image = checkimage(L);
	image->rotation = (GLdouble)luaL_checknumber(L, 2);
	return 0;
}

static int Lua_Image_getRotation(lua_State *L) {
	Lua_Image *image = checkimage(L);
	lua_pushnumber(L, image->rotation);
	return 1;
}

static int Lua_Image_setRect(lua_State *L) {
	Lua_Image *image = checkimage(L);
	int x = luaL_checkint(L, 2);
	int y = luaL_checkint(L, 3);
	int w = luaL_checkint(L, 4);
	int h = luaL_checkint(L, 5);
	if (image->rect == NULL) {
		image->rect = (myRect *)malloc(sizeof(myRect));
	}
	image->rect->x = x;
	image->rect->y = y;
	image->rect->w = w;
	image->rect->h = h;
	return 0;
}

static int Lua_Image_getRect(lua_State *L) {
	Lua_Image *image = checkimage(L);
	if (image->rect == NULL) {
		lua_pushinteger(L, 0);
		lua_pushinteger(L, 0);
		lua_pushinteger(L, image->w);
		lua_pushinteger(L, image->h);
	} else {
		lua_pushinteger(L, image->rect->x);
		lua_pushinteger(L, image->rect->y);
		lua_pushinteger(L, image->rect->w);
		lua_pushinteger(L, image->rect->h);
	}
	return 4;
}

static int Lua_Image_clearRect(lua_State *L) {
	Lua_Image *image = checkimage(L);
	/* if image->rect is NULL, no operation is performed */
	free(image->rect);
	image->rect = NULL;
	return 0;
}

static int Lua_Image_render(lua_State *L) {
	Lua_Image *image = checkimage(L);
	int narg;
	myRect clip_rect;
	int color[] = {255, 255, 255, image->alpha};
	GLrect texCoords = {0, 0, 0, 0};
	int width = image->tile_width;
	int height = image->tile_height;
	GLdouble x = image->x;
	GLdouble y = image->y;
	GLdouble translate_x = image->center_x;
	GLdouble translate_y = image->center_y;
	GLdouble scale_x = image->scale_x;
	GLdouble scale_y = image->scale_y;
	GLdouble rotation = image->rotation;
	GLubyte x_tile, y_tile;
	GLuint *textures = image->textures;
	
	texCoords.x2 = (GLfloat) image->po2_width / width;
	texCoords.y2 = (GLfloat) image->po2_height / height;

	if (image->color != NULL) {
		color[0] = image->color[0];
		color[1] = image->color[1];
		color[2] = image->color[2];
	}
	
	if (image->rect != NULL) {
		texCoords.x1 = (GLfloat) image->rect->x / image->po2_width;
		texCoords.y1 = (GLfloat) image->rect->y / image->po2_height;
		texCoords.x2 = (GLfloat) (image->rect->x + image->rect->w) / image->po2_width;
		texCoords.y2 = (GLfloat) (image->rect->y + image->rect->h) / image->po2_height;
		width = image->rect->w;
		height = image->rect->h;
	}
	
	narg = lua_gettop(L);
	if (narg == 2 && lua_istable(L, 2)) {
		/* check for "x" and "y" entries */
		lua_getfield(L, -1, "x");
		if (lua_isnumber(L, -1))
			x = (GLdouble)lua_tonumber(L, -1);
		lua_getfield(L, -2, "y");
		if (lua_isnumber(L, -1))
			y = (GLdouble)lua_tonumber(L, -1);
		lua_pop(L, 2);
		/* check for "centerX" entry */
		lua_getfield(L, -1, "centerX");
		if (lua_isnumber(L, -1))
			translate_x = (GLdouble)lua_tonumber(L, -1);
		lua_pop(L, 1);
		/* check for "centerY" entry */
		lua_getfield(L, -1, "centerY");
		if (lua_isnumber(L, -1))
			translate_y = (GLdouble)lua_tonumber(L, -1);
		lua_pop(L, 1);
		/* check for "scaleX" entry */
		lua_getfield(L, -1, "scaleX");
		if (lua_isnumber(L, -1))
			scale_x = (GLdouble)lua_tonumber(L, -1);
		lua_pop(L, 1);
		/* check for "scaleY" entry */
		lua_getfield(L, -1, "scaleY");
		if (lua_isnumber(L, -1))
			scale_y = (GLdouble)lua_tonumber(L, -1);
		lua_pop(L, 1);
		/* check for "rotate" entry */
		lua_getfield(L, -1, "rotate");
		if (lua_isnumber(L, -1))
			rotation = (GLdouble)lua_tonumber(L, -1);
		lua_pop(L, 1);
		/* check for "rect" entry */
		lua_getfield(L, -1, "rect");
		if (lua_istable(L, -1)) {
			if (!getint(L, &clip_rect.x, 1) || !getint(L, &clip_rect.y, 2) ||
			    !getint(L, &clip_rect.w, 3) || !getint(L, &clip_rect.h, 4))
				return luaL_argerror(L, 2, "'rect' has invalid or missing elements");

			texCoords.x1 = (GLfloat) clip_rect.x / image->po2_width;
			texCoords.y1 = (GLfloat) clip_rect.y / image->po2_height;
			texCoords.x2 = (GLfloat) (clip_rect.x + clip_rect.w) / image->po2_width;
			texCoords.y2 = (GLfloat) (clip_rect.y + clip_rect.h) / image->po2_height;
			width = clip_rect.w;
			height = clip_rect.h;
		} else if (!lua_isnil(L, -1))
			return luaL_argerror(L, 2, "'rect' should be an array and nothing else");
		lua_pop(L, 1);
		/* check for "color" entry */
		lua_getfield(L, -1, "color");
		if (lua_istable(L, -1)) {
			if (!getint(L, &color[0], 1) || !getint(L, &color[1], 2) || !getint(L, &color[2], 3))
				return luaL_argerror(L, 1, "'color' has invalid or missing elements");
			/* the alpha value of the color is optional */
			getint(L, &color[3], 4);
		} else if (!lua_isnil(L, -1))
			return luaL_argerror(L, 1, "'color' should be an array and nothing else");
	} else if (narg == 3 && lua_isnumber(L, 2) && lua_isnumber(L, 3)) {
		x = lua_tonumber(L, 2);
		y = lua_tonumber(L, 3);
	} else if (narg != 1) {
		return luaL_error(L, "Wrong type of argument(s).");
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	/* blit the OpenGL texture (wrapped in a Lua_Image) */
	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);

	/* save the modelview matrix */
	glPushMatrix();
	glTranslated(x, y, 0);
	glScaled(scale_x, scale_y, 0);
	glRotated(rotation, 0, 0, 1);
	glTranslated(-translate_x, -translate_y, 0);

	glColor4ub( (GLubyte)color[0], (GLubyte)color[1], (GLubyte)color[2], (GLubyte)color[3]);
	
	for (y_tile=0; y_tile<image->y_tiles; y_tile++) {
		glPushMatrix();
		for (x_tile=0; x_tile<image->x_tiles; x_tile++) {
			glBindTexture(GL_TEXTURE_2D, *textures++);
			glBegin(GL_QUADS);
			if (scale_x*scale_y > 0) {
				glTexCoord2f(texCoords.x1, texCoords.y1);
				glVertex2i(0, 0);
				glTexCoord2f(texCoords.x1, texCoords.y2);
				glVertex2i(0, height);
				glTexCoord2f(texCoords.x2, texCoords.y2);
				glVertex2i(width, height);
				glTexCoord2f(texCoords.x2, texCoords.y1);
				glVertex2i(width, 0);
			} else {
				glTexCoord2f(texCoords.x1, texCoords.y1);
				glVertex2i(0, 0);
				glTexCoord2f(texCoords.x2, texCoords.y1);
				glVertex2i(width, 0);
				glTexCoord2f(texCoords.x2, texCoords.y2);
				glVertex2i(width, height);
				glTexCoord2f(texCoords.x1, texCoords.y2);
				glVertex2i(0, height);
			}
			glEnd();
			glTranslatef((GLfloat)image->tile_width-0.0f, 0.0f, 0.0f);
		}
		glPopMatrix();
		glTranslatef(0.0f, (GLfloat)image->tile_height-0.0f, 0.0f);
	}

	/* restore the modelview matrix */
	glPopMatrix();

	return 0;
}

static int lua_buffer_write(SDL_RWops *context, const void *ptr, int size, int num) {
	luaL_Buffer *pb = (luaL_Buffer *)context->hidden.unknown.data1;
	luaL_addlstring(pb, ptr, num*size);
	return num;
}

static int Lua_Image_toString(lua_State *L) {
	Lua_Image *image = checkimage(L);
	SDL_RWops *rwops;

	luaL_Buffer b;
	luaL_buffinit(L, &b);
	rwops = SDL_AllocRW();
	if (rwops == NULL) {
		return luaL_error(L, "Couldn't allocate SDL_RWops: %s", SDL_GetError());
	}
	/* this SDL_RWops supports writing only */
	rwops->seek = NULL;
	rwops->read = NULL;
	rwops->write = lua_buffer_write;
	rwops->close = NULL;
	rwops->hidden.unknown.data1 = &b;

	IMG_SavePNG_RW(rwops, image->src, IMG_COMPRESS_MAX);

	luaL_pushresult(&b);

	SDL_FreeRW(rwops);

	return 1;
}

static int image_gc(lua_State *L) {
	Lua_Image *image = checkimage(L);
	glDeleteTextures( image->x_tiles*image->y_tiles, image->textures );
	free(image->textures);
	/* if the pointer is NULL, no operation is performed */
	free(image->color);
	free(image->rect);
	SDL_FreeSurface(image->src);
	/* remove image from list */
	if (image->prev == NULL) {
		first_image = image->next;
	} else {
		image->prev->next = image->next;
	}
	if (image->next != NULL) {
		image->next->prev = image->prev;
	}
	/* fprintf(stdout, "Freed image.\n"); */
	return 0;
}

static int image_tostring(lua_State *L) {
	lua_pushliteral(L, "Image");
	return 1;
}

static int Lua_Graphics_draw(lua_State *L) {
	int i;
	int n;
	double *coords;
	int relative;
	GLdouble translate_x = 0.0;
	GLdouble translate_y = 0.0;
	GLdouble scale_x = 1.0;
	GLdouble scale_y = 1.0;
	GLdouble rotate = 0.0;
	GLdouble center_x = 0.0;
	GLdouble center_y = 0.0;
	int color[] = {255, 255, 255, 255};
	GLfloat size = 1.0f;
	int connect = 0;
	int fill = 0;
	int pixellist = 0;
	int antialias = 0;

	luaL_checktype(L, 1, LUA_TTABLE);
	/* get number of elements in the array part of the table */
	n = lua_objlen(L, 1);
	luaL_argcheck(L, n % 2 == 0, 1, "even number of x- and y-coordinates needed");

	/* collect all coordinates in array */
	coords = (double *)malloc(n*sizeof(double));
	for (i=0; i<n; i++) {
		if (!getdouble(L, &coords[i], i+1)) {
			free(coords);
			return luaL_argerror(L, 1, "invalid x or y component in array");
		}
	}
	/* check for "relative"-entry */
	lua_getfield(L, -1, "relative");
	relative = lua_toboolean(L, -1);
	if (relative) {
		n = n - 2;
		if (n<0) {
			free(coords);
			return luaL_argerror(L, 1, "'relative' needs at least one coordinate pair");
		}
		translate_x = (GLdouble)coords[0];
		translate_y = (GLdouble)coords[1];
	}
	lua_pop(L, 1);
	/* check for "centerX"-entry */
	lua_getfield(L, -1, "centerX");
	if (lua_isnumber(L, -1))
		center_x = (GLdouble)lua_tonumber(L, -1);
	lua_pop(L, 1);
	/* check for "centerY"-entry */
	lua_getfield(L, -1, "centerY");
	if (lua_isnumber(L, -1))
		center_y = (GLdouble)lua_tonumber(L, -1);
	lua_pop(L, 1);
	/* check for "scaleX"-entry */
	lua_getfield(L, -1, "scaleX");
	if (lua_isnumber(L, -1))
		scale_x = (GLdouble)lua_tonumber(L, -1);
	lua_pop(L, 1);
	/* check for "scaleY"-entry */
	lua_getfield(L, -1, "scaleY");
	if (lua_isnumber(L, -1))
		scale_y = (GLdouble)lua_tonumber(L, -1);
	lua_pop(L, 1);
	/* check for "rotate"-entry */
	lua_getfield(L, -1, "rotate");
	if (lua_isnumber(L, -1))
		rotate = (GLdouble)lua_tonumber(L, -1);
	lua_pop(L, 1);
	/* check for "color"-entry */
	lua_getfield(L, -1, "color");
	if (lua_istable(L, -1)) {
		if (!getint(L, &color[0], 1) || !getint(L, &color[1], 2) || !getint(L, &color[2], 3)) {
			free(coords);
			return luaL_argerror(L, 1, "'color' has invalid or missing elements");
		}
		/* the alpha value of the color is optional */
		getint(L, &color[3], 4);
	}
	lua_pop(L, 1);
	/* check for "size"-entry */
	lua_getfield(L, -1, "size");
	if (lua_isnumber(L, -1))
		size = (GLfloat)lua_tonumber(L, -1);
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
	pixellist = lua_toboolean(L, -1);
	lua_pop(L, 1);

	if (!connect && !fill && !pixellist && (n % 4 != 0) && (n > 2)) {
		free(coords);
		luaL_argerror(L, 1, "unconnected lines need an even number of coordinate-pairs");
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	/* save the modelview matrix */
	glPushMatrix();
	
	/* some translation by half pixels to get the coordinates the user intends to use */
	if (n == 2 || pixellist) {
		glTranslated(0.5, 0.5, 0.0);
	} else if (fill) {
		if (n == 4) {
			glTranslated(0.5, 0.5, 0.0);
		}
	} else {
		glTranslated(0.5, 0.5, 0.0);
	}
	
	glTranslated(translate_x, translate_y, 0);
	glScaled(scale_x, scale_y, 0);
	glRotated(rotate, 0, 0, 1);
	glTranslated(-center_x, -center_y, 0);

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
	if (n == 2 || pixellist) {
		glBegin(GL_POINTS);
	} else if (fill) {
		if (n == 4) {
			glBegin(GL_LINE_STRIP);
		}
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

	for (i=n+2*(relative-1); i>=2*relative; i=i-2) {
		glVertex2d(coords[i], coords[i+1]);
	}

	glEnd();

	/* restore the modelview matrix */
	glPopMatrix();

	free(coords);
	return 0;
}

static const struct luaL_Reg graphicslib [] = {
	{"getFullscreenModes", Lua_Graphics_getFullscreenModes},
	{"init",               Lua_Graphics_init},
	{"setBackgroundColor", Lua_Graphics_setBackgroundColor},
	{"getBackgroundColor", Lua_Graphics_getBackgroundColor},
	{"getWindowWidth",     Lua_Graphics_getWindowWidth},
	{"getWindowHeight",    Lua_Graphics_getWindowHeight},
	{"getWindowSize",      Lua_Graphics_getWindowSize},
	{"showCursor",         Lua_Graphics_showCursor},
	{"getTicks",           Lua_Graphics_getTicks},
	{"translateView",      Lua_Graphics_translateView},
	{"scaleView",          Lua_Graphics_scaleView},
	{"rotateView",         Lua_Graphics_rotateView},
	{"saveView",           Lua_Graphics_saveView},
	{"restoreView",        Lua_Graphics_restoreView},
	{"resetView",          Lua_Graphics_resetView},
	{"addImage",           Lua_Image_load},
	{"addImageFromString", Lua_Image_loadFromString},
	{"addImageFromCairo",  Lua_Image_loadFromCairoSurface},
	{"draw",               Lua_Graphics_draw},
	{NULL, NULL}
};

static const struct luaL_Reg imagelib_m [] = {
	{"__gc",          image_gc},
	{"__tostring",    image_tostring},
	{"getWidth",      Lua_Image_getWidth},
	{"getHeight",     Lua_Image_getHeight},
	{"getSize",	      Lua_Image_getSize},
	{"isTransparent", Lua_Image_isTransparent},
	{"setAlpha",      Lua_Image_setAlpha},
	{"getAlpha",      Lua_Image_getAlpha},
	{"setColor",      Lua_Image_setColor},
	{"getColor",      Lua_Image_getColor},
	{"clearColor",    Lua_Image_clearColor},
	{"setX",          Lua_Image_setX},
	{"getX",          Lua_Image_getX},
	{"setY",          Lua_Image_setY},
	{"getY",          Lua_Image_getY},
	{"setPosition",   Lua_Image_setPosition},
	{"getPosition",   Lua_Image_getPosition},
	{"setCenterX",    Lua_Image_setCenterX},
	{"getCenterX",    Lua_Image_getCenterX},
	{"setCenterY",    Lua_Image_setCenterY},
	{"getCenterY",    Lua_Image_getCenterY},
	{"setCenter",     Lua_Image_setCenter},
	{"getCenter",     Lua_Image_getCenter},
	{"setScaleX",     Lua_Image_setScaleX},
	{"getScaleX",     Lua_Image_getScaleX},
	{"setScaleY",     Lua_Image_setScaleY},
	{"getScaleY",     Lua_Image_getScaleY},
	{"setScale",      Lua_Image_setScale},
	{"getScale",      Lua_Image_getScale},
	{"setRotation",   Lua_Image_setRotation},
	{"getRotation",   Lua_Image_getRotation},
	{"setRect",       Lua_Image_setRect},
	{"getRect",       Lua_Image_getRect},
	{"clearRect",     Lua_Image_clearRect},
	{"toString",      Lua_Image_toString},
	{"render",        Lua_Image_render},
	{NULL, NULL}
};

int luaopen_oocairo_wrapper(lua_State *L) {
	/* call the original luaopen_oocairo */
	lua_pushcfunction(L, luaopen_oocairo);
	lua_insert(L, -2);
	lua_call(L, 1, 1);
	
	/* add method "render" to Cairo surface objects */
	luaL_getmetatable(L, "6d31a064-6711-11dd-bdd8-00e081225ce5");
	lua_pushliteral(L, "render");
	lua_pushcfunction(L, Lua_Cairo_renderSurface);
	lua_rawset(L, -3);
	lua_pop(L, 1);
	
	return 1;
}

int luaopen_graphics(lua_State *L, const char *parent) {
	if ( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK ) != 0 ) {
		error(L, "Couldn't initialize SDL: %s", SDL_GetError ());
	}

	luaL_newmetatable(L, "scrupp.image");
	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);	/* duplicates the metatable */
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, imagelib_m);
	lua_pop(L, 1);	/* pop the metatable */
	luaL_register(L, parent, graphicslib);

	return 1;
}
