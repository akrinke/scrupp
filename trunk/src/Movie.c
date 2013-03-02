/*
** $Id$
** Library for MPEG support
** See Copyright Notice in COPYRIGHT
*/

#include <smpeg/smpeg.h>
#include <SDL_mixer.h>

#include "Main.h"
#include "Macros.h"
#include "Movie.h"
#include "Graphics.h"
#include "physfsrwops.h"

#define tomoviep(L) \
	(Lua_Movie **)luaL_checkudata(L, 1, "scrupp.movie")

Lua_Movie *first_movie = NULL;
SDL_mutex *movie_mutex;

static Lua_Movie *tomovie(lua_State *L) {
	Lua_Movie **movie = tomoviep(L);
	if (*movie == NULL) {
		luaL_error(L, "attempt to use a removed movie");
	}
	return *movie;
}

static void update_callback(SDL_Surface *dst, int x, int y, unsigned int w, unsigned int h) {
	Lua_Movie *node = first_movie;
	while (node != NULL) {
		if (node->surface == dst) {
			node->changed = 1;
			break;
		}
	}
}

static int Lua_Movie_load(lua_State *L) {
	const char* filename = luaL_checkstring(L, 1);
	GLuint texture;
	SDL_RWops *temp;
	SMPEG *movie;
	SMPEG_Info *movie_info;
	int po2_width, po2_height;
	SDL_Surface *movie_surface;
	SDL_Surface *po2_surface;
	Lua_Movie **ptr;

	if (SDL_GetVideoSurface() == NULL) {
		return luaL_error(L, "Run " NAMESPACE ".init() before loading movies.");
	}

	movie_info = (SMPEG_Info *) malloc(sizeof(SMPEG_Info));
	temp = PHYSFSRWOPS_openRead(filename);
	if (!temp) {
		free(movie_info);
		return luaL_error(L, "Error loading file '%s': %s", filename, SDL_GetError());
	}
	
	movie = SMPEG_new_rwops(temp, movie_info, 0);
	if (!movie) {
		free(movie_info);
		return luaL_error(L, "Error loading file '%s': %s", filename, SMPEG_error(movie));
	}

	po2_width = nextHigherPowerOfTwo(movie_info->width);
	po2_height = nextHigherPowerOfTwo(movie_info->height);
	
	/* the movie surface has to have the same size as the movie (?) */
	movie_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
		movie_info->width, movie_info->height, 32,
		RMASK, GMASK, BMASK, AMASK);
	
	/* the texture surface needs a power of 2 as width and height */
	po2_surface = SDL_CreateRGBSurface(SDL_SWSURFACE, 
		po2_width, po2_height, 32,
		RMASK, GMASK, BMASK, AMASK);
	
	if ((movie_surface == NULL) || (po2_surface == NULL)) {
		if (movie_surface != NULL)
			SDL_FreeSurface(movie_surface);
		if (po2_surface != NULL)
			SDL_FreeSurface(po2_surface);
		free(movie_info);
		return luaL_error(L, "Error loading file '%s': %s", filename, SDL_GetError());
	}

	SMPEG_setdisplay(movie, movie_surface, movie_mutex, update_callback);
	SMPEG_enablevideo(movie, 1);
	SMPEG_enableaudio(movie, 0);
	
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(	GL_TEXTURE_2D, 0, GL_RGB, po2_width, po2_height, 0, 
					GL_RGBA, GL_UNSIGNED_BYTE, po2_surface->pixels );
	
	SDL_FreeSurface(po2_surface);

	ptr = (Lua_Movie **)lua_newuserdata(L, sizeof(Lua_Movie *));
	*ptr = NULL;
	*ptr = (Lua_Movie *)malloc(sizeof(Lua_Movie));
	(*ptr)->texture = texture;
	(*ptr)->po2_width = po2_width;
	(*ptr)->po2_height = po2_height;
	(*ptr)->info = movie_info;
	(*ptr)->movie = movie;	
    (*ptr)->surface = movie_surface;
	(*ptr)->alpha = 255;
	(*ptr)->music_hooked = 0;
	(*ptr)->changed = 1;
	(*ptr)->next = NULL;
	(*ptr)->prev = NULL;
	
	if (first_movie == NULL) {
		first_movie = *ptr;
	} else {
		first_movie->prev = *ptr;
		(*ptr)->next = first_movie;
		first_movie = *ptr;
	}
	
	luaL_getmetatable(L, "scrupp.movie");
	lua_setmetatable(L, -2);
	
	return 1;
}

static int Lua_Movie_play(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	int loop = lua_toboolean(L, 2);
	SDL_AudioSpec audiofmt;
	Uint16 format;
	int freq, channels;

	if (movie->info->has_audio) {
		SMPEG_enableaudio(movie->movie, 0);
	
		/* Tell SMPEG what the audio format is */
		Mix_QuerySpec(&freq, &format, &channels);
		audiofmt.freq = freq;
		audiofmt.format = format;	
		audiofmt.channels = channels;
		/* printf("freq: %d format: %d channels: %d\n", freq, format, channels); */
		SMPEG_actualSpec(movie->movie, &audiofmt);

		/* Hook in the MPEG music mixer */
		Mix_HookMusic(SMPEG_playAudioSDL, movie->movie);
		movie->music_hooked = 1;
		SMPEG_enableaudio(movie->movie, 1);
	}

	SMPEG_loop(movie->movie, loop);
	SMPEG_play(movie->movie);

	return 0;
}

static int Lua_Movie_pause(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	if (SMPEG_status(movie->movie) == SMPEG_PLAYING) {
		SMPEG_pause(movie->movie);
	}
	return 0;
}

static int Lua_Movie_resume(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	if (SMPEG_status(movie->movie) == SMPEG_STOPPED) {
		SMPEG_pause(movie->movie);
	}
	return 0;
}

static int Lua_Movie_rewind(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	SMPEG_rewind(movie->movie);
	return 0;
}

static int Lua_Movie_stop(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	SMPEG_stop(movie->movie);
	Mix_HookMusic(NULL, NULL);
	movie->music_hooked = 0;
	return 0;
}

static int Lua_Movie_skip(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	float seconds = (float)luaL_checknumber(L, 2);
	SMPEG_skip(movie->movie, seconds);
	return 0;
}

static int Lua_Movie_getWidth(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	lua_pushinteger(L, movie->info->width);
	return 1;
}

static int Lua_Movie_getHeight(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	lua_pushinteger(L, movie->info->height);
	return 1;
}

static int Lua_Movie_getSize(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	lua_pushinteger(L, movie->info->width);
	lua_pushinteger(L, movie->info->height);
	return 2;
}

static int Lua_Movie_getInfo(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	SMPEG_Info info;
	SMPEG_status(movie->movie);
	movie->changed=1;
	SMPEG_getinfo(movie->movie, &info);
	lua_createtable(L, 0, 10);

	lua_pushliteral(L, "hasAudio");
	lua_pushboolean(L, info.has_audio);
	lua_rawset(L, -3);

	lua_pushliteral(L, "hasVideo");
	lua_pushboolean(L, info.has_video);
	lua_rawset(L, -3);

	lua_pushliteral(L, "currentFrame");
	lua_pushinteger(L, info.current_frame);
	lua_rawset(L, -3);

	lua_pushliteral(L, "currentFPS");
	lua_pushnumber(L, info.current_fps);
	lua_rawset(L, -3);

	lua_pushliteral(L, "audioString");
	lua_pushstring(L, info.audio_string);
	lua_rawset(L, -3);

	lua_pushliteral(L, "currentAudioFrame");
	lua_pushinteger(L, info.audio_current_frame);
	lua_rawset(L, -3);

	lua_pushliteral(L, "currentOffset");
	lua_pushinteger(L, info.current_offset);
	lua_rawset(L, -3);

	lua_pushliteral(L, "totalSize");
	lua_pushinteger(L, info.total_size);
	lua_rawset(L, -3);

	lua_pushliteral(L, "currentTime");
	lua_pushnumber(L, info.current_time);
	lua_rawset(L, -3);

	lua_pushliteral(L, "totalTime");
	lua_pushnumber(L, info.total_time);
	lua_rawset(L, -3);

	return 1;
}

static int Lua_Movie_isPlaying(lua_State *L) {
	Lua_Movie *movie = tomovie(L);	
	lua_pushboolean(L, SMPEG_status(movie->movie) == SMPEG_PLAYING);
	return 1;
}

static int Lua_Movie_setAlpha(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	int alpha = luaL_checkint(L, 2);
	movie->alpha = (GLubyte) alpha;
	return 0;
}

static int Lua_Movie_getAlpha(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	lua_pushinteger(L, movie->alpha);
	return 1;
}

static int Lua_Movie_loadFirstFrame(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	SMPEG_renderFrame(movie->movie, 1);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, movie->surface->w, movie->surface->h,
					GL_RGBA, GL_UNSIGNED_BYTE, movie->surface->pixels);
	return 0;
}

static int Lua_Movie_render(lua_State *L) {
	Lua_Movie *movie = tomovie(L);
	int x, y;
	myRect clip_rect;
	int color[] = {255, 255, 255, 255};
	GLrect texCoords = {0.0, 0.0, 0.0, 0.0};
	int width = movie->info->width;
	int height = movie->info->height;
	GLdouble translateX = 0.0;
	GLdouble translateY = 0.0;
	GLdouble scaleX = 1.0;
	GLdouble scaleY = 1.0;
	GLdouble rotate = 0.0;
	SMPEGstatus status;

	texCoords.x2 = (GLfloat) width / movie->po2_width;
	texCoords.y2 = (GLfloat) height / movie->po2_height;
	
	if (lua_istable(L, 2)) {
		/* x and y are array element 1 and 2 */
		if (!getint(L, &x, 1) || !getint(L, &y, 2)) {
			return luaL_argerror(L, 2, "invalid x or y component in array");
		}
		/* check for "centerX"-entry */
		lua_getfield(L, -1, "centerX");
		if (lua_isnumber(L, -1)) {
			translateX = (GLdouble)lua_tonumber(L, -1);
		}
		lua_pop(L, 1);
		/* check for "centerY"-entry */
		lua_getfield(L, -1, "centerY");
		if (lua_isnumber(L, -1))
			translateY = (GLdouble)lua_tonumber(L, -1);
		lua_pop(L, 1);
		/* check for "scaleX"-entry */
		lua_getfield(L, -1, "scaleX");
		if (lua_isnumber(L, -1)) {
			scaleX = (GLdouble)lua_tonumber(L, -1);
		}
		lua_pop(L, 1);
		/* check for "scaleY"-entry */
		lua_getfield(L, -1, "scaleY");
		if (lua_isnumber(L, -1)) {
			scaleY = (GLdouble)lua_tonumber(L, -1);
		}
		lua_pop(L, 1);
		/* check for "rotate"-entry */
		lua_getfield(L, -1, "rotate");
		if (lua_isnumber(L, -1)) {
			rotate = (GLdouble)lua_tonumber(L, -1);
		}
		lua_pop(L, 1);
		/* check for "rect"-entry */
		lua_getfield(L, -1, "rect");
		if (lua_istable(L, -1)) {
			if (!getint(L, &clip_rect.x, 1) || !getint(L, &clip_rect.y, 2) ||
				!getint(L, &clip_rect.w, 3) || !getint(L, &clip_rect.h, 4)) {
				return luaL_argerror(L, 2, "'rect' has invalid or missing elements");
			}

			texCoords.x1 = (GLfloat) clip_rect.x / movie->surface->w;
			texCoords.y1 = (GLfloat) clip_rect.y / movie->surface->h;
			texCoords.x2 = (GLfloat) (clip_rect.x + clip_rect.w) / movie->surface->w;
			texCoords.y2 = (GLfloat) (clip_rect.y + clip_rect.h) / movie->surface->h;
			width = clip_rect.w;
			height = clip_rect.h;
		} else if (!lua_isnil(L, -1)) {
			return luaL_argerror(L, 2, "'rect' should be an array and nothing else");
		}
		lua_pop(L, 1);
		/* check for "color"-entry */
		lua_getfield(L, -1, "color");
		if (lua_istable(L, -1)) {
			if (!getint(L, &color[0], 1) || !getint(L, &color[1], 2) || !getint(L, &color[2], 3)) {
				return luaL_argerror(L, 1, "'color' has invalid or missing elements");
			}
			/* the alpha value of the color is optional */
			getint(L, &color[3], 4);
			movie->alpha = (GLubyte)color[3];
		} else if (!lua_isnil(L, -1)) {
			return luaL_argerror(L, 1, "'color' should be an array and nothing else");
		}
	} else {
		x = luaL_checkint(L, 2);
		y = luaL_checkint(L, 3);
	}

	/* needed to restart the video if it should loop */
	status = SMPEG_status(movie->movie);

	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, movie->texture);

	/* save the modelview matrix */
	glPushMatrix();
	glTranslatef((GLfloat)x, (GLfloat)y, 0);
	glScaled(scaleX, scaleY, 0);
	glRotated(rotate, 0, 0, 1);
	glTranslated(-translateX, -translateY, 0);

	glColor4ub( (GLubyte)color[0], (GLubyte)color[1], (GLubyte)color[2], movie->alpha);

	if ((status == SMPEG_PLAYING) && (movie->changed == 1)) {
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, movie->surface->w, movie->surface->h,
						GL_RGBA, GL_UNSIGNED_BYTE, movie->surface->pixels);
		movie->changed = 0;
	}

	glBegin(GL_QUADS);
		if (scaleX*scaleY > 0) {			
			glTexCoord2f(texCoords.x1, texCoords.y1);
			glVertex2i(0, 0);
			glTexCoord2f(texCoords.x1, texCoords.y2);
			glVertex2i(0, height);
			glTexCoord2f( texCoords.x2, texCoords.y2);
			glVertex2i(width, height);
			glTexCoord2f(texCoords.x2, texCoords.y1);
			glVertex2i(width, 0);
		} else {
			glTexCoord2f(texCoords.x1, texCoords.y1);
			glVertex2i(0, 0);
			glTexCoord2f(texCoords.x2, texCoords.y1);
			glVertex2i(width, 0);
			glTexCoord2f( texCoords.x2, texCoords.y2);
			glVertex2i(width, height);
			glTexCoord2f(texCoords.x1, texCoords.y2);
			glVertex2i(0, height);	
		}
	glEnd();

	/* restore the modelview matrix */
	glPopMatrix();

	return 0;
}

static int movie_gc(lua_State *L) {
	/* stack(-1): movie userdata to free */
	Lua_Movie **movie = tomoviep(L);
	if (*movie != NULL) {
		if ((*movie)->music_hooked) {
			Mix_HookMusic(NULL, NULL);
		}
		SMPEG_delete((*movie)->movie);
		glDeleteTextures( 1, &(*movie)->texture );
		SDL_FreeSurface((*movie)->surface);
		free((*movie)->info);

		/* remove movie from list */
		if ((*movie)->prev == NULL) {
			first_movie = (*movie)->next;
		} else {
			(*movie)->prev->next = (*movie)->next;
		}
		if ((*movie)->next != NULL) {
			(*movie)->next->prev = (*movie)->prev;
		}
		
		free(*movie);
		*movie = NULL;
		/* fprintf(stdout, "Freed movie.\n"); */
	}
	return 0;
}

static int movie_tostring(lua_State *L) {
	Lua_Movie *movie = *tomoviep(L);
	if (movie == NULL) {
		lua_pushliteral(L, "Movie (removed)");
	} else {
		lua_pushfstring(L, "Movie (%p)", movie);
	}
	return 1;
}

static const struct luaL_Reg movielib [] = {
	{"addMovie", Lua_Movie_load},
	{NULL, NULL}
};

static const struct luaL_Reg movielib_m [] = {
	{"__gc", movie_gc},
	{"__tostring", movie_tostring},
	{"play", Lua_Movie_play},
	{"pause", Lua_Movie_pause},
	{"resume", Lua_Movie_resume},
	{"rewind", Lua_Movie_rewind},
	{"stop", Lua_Movie_stop},
	{"skip", Lua_Movie_skip},
	{"getWidth", Lua_Movie_getWidth},
	{"getHeight", Lua_Movie_getHeight},
	{"getSize", Lua_Movie_getSize},
	{"getInfo", Lua_Movie_getInfo},
	{"isPlaying", Lua_Movie_isPlaying},
	{"setAlpha", Lua_Movie_setAlpha},
	{"getAlpha", Lua_Movie_getAlpha},
	{"loadFirstFrame", Lua_Movie_loadFirstFrame},
	{"render", Lua_Movie_render},
	{"remove", movie_gc},
	{NULL, NULL}
};

int luaopen_movie(lua_State *L, const char *parent) {
	luaL_newmetatable(L, "scrupp.movie");
	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);	/* duplicates the metatable */
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, movielib_m);
	lua_pop(L, 1);	/* pop the metatable */
	luaL_register(L, parent, movielib);
	movie_mutex = SDL_CreateMutex();
	return 1;
}


