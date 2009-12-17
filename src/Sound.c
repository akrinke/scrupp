/*
** $Id$
** Library for sound and music with Scrupp
** See Copyright Notice in COPYRIGHT
*/

#include <SDL_mixer.h>

#include "Main.h"
#include "Macros.h"
#include "Sound.h"
#include "physfsrwops.h"

#define checksound(L) \
	(Mix_Chunk **)luaL_checkudata(L, 1, "scrupp.sound")

#define checkmusic(L) \
	(Mix_Music **)luaL_checkudata(L, 1, "scrupp.music")

/* this array connects the loaded samples with the channels they are played on */
static Mix_Chunk* channels[CHANNELS];

static void channelDone(int channel) {
	/* printf("channel %d finished playback.\n", channel); */
	channels[channel] = NULL;
}

static int initSound(void) {
	int i;
	/* 	open 44.1KHz, signed 16bit, system byte order,
		stereo audio, using 1024 byte chunks */
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024)==-1) {
		return -1;
	}
	Mix_AllocateChannels(CHANNELS);
	for (i=0; i<CHANNELS; i++) {
		channels[i] = NULL;
	}
	Mix_ChannelFinished(channelDone);
	atexit(Mix_CloseAudio);
	return 0;
}

/*******************************************
	Sound functions
********************************************/
static int Lua_Sound_load(lua_State *L) {
	Mix_Chunk *sample;
	Mix_Chunk **ptr;
	SDL_RWops *src;
	const char* filename = luaL_checkstring(L, 1);

	src = PHYSFSRWOPS_openRead(filename);
	if (!src) {
		return luaL_error(L, "Error loading sound '%s': %s", filename, SDL_GetError());
	}
	sample = Mix_LoadWAV_RW(src, 1);
	if (!sample) {
		return luaL_error(L, "Error loading sound '%s': %s", filename, Mix_GetError());
	}
	/* new userdata for pointer to sample */
	ptr = lua_newuserdata(L, sizeof(Mix_Chunk*));
	*ptr = sample;
	luaL_getmetatable(L, "scrupp.sound");
	lua_setmetatable(L, -2);
	return 1;
}

static int Lua_Sound_play(lua_State *L) {
	Mix_Chunk **sample = checksound(L);
	int channel;
	int loops = luaL_optint(L, 2, 1);	/* default: play sample one time */
	
	/* fade-in functionality has been disabled due to bugs in SDL-Mixer 1.2.8 */
	/* int ms = luaL_optint(L, 3, 0); */	/* default: no fade-in */
	luaL_argcheck(L, loops>=0, 2, "number of loops has to be positive");
	/* luaL_argcheck(L, ms>=0, 3, "Fade-in time has to be positive"); */
	/* int channel = Mix_FadeInChannel(-1, *sample, loops-1, 100); */
	channel = Mix_PlayChannel(-1, *sample, loops-1);
	if (channel == -1) {
		fprintf(stderr, "Error playing sound: %s\n", Mix_GetError());
	} else {
		channels[channel] = *sample;
	}
	return 0;
}

static int Lua_Sound_setVolume(lua_State *L) {
	Mix_Chunk **sample = checksound(L);
	int volume = luaL_checkint(L, 2);
	luaL_argcheck(L, (volume>=0) && (volume<=MIX_MAX_VOLUME), 1, "0 <= volume <= " xstr(MIX_MAX_VOLUME));
	Mix_VolumeChunk(*sample, volume);
	return 0;
}

static int Lua_Sound_getVolume(lua_State *L) {
	Mix_Chunk **sample = checksound(L);
	lua_pushinteger(L, Mix_VolumeChunk(*sample, -1));
	return 1;
}

static int Lua_Sound_pause (lua_State *L) {
	Mix_Chunk **sample = checksound(L);
	int i;
	for (i=0; i<CHANNELS; i++) {
		if (channels[i] == *sample) {
			Mix_Pause(i);
		}
	}
	return 0;
}

static int Lua_Sound_resume (lua_State *L) {
	Mix_Chunk **sample = checksound(L);
	int i;
	for (i=0; i<CHANNELS; i++) {
		if (channels[i] == *sample) {
			Mix_Resume(i);
		}
	}
	return 0;
}

static int Lua_Sound_stop (lua_State *L) {
	Mix_Chunk **sample = checksound(L);
	
	/* fade-out functionality has been disabled due to bugs in SDL-Mixer 1.2.8 */
	/* int ms = luaL_optint(L, 2, 0); */	/* default: no fade-out */
	/*luaL_argcheck(L, ms>=0, 2, "Fade-out time has to be positive"); */
	int i;
	for (i=0; i<CHANNELS; i++) {
		if (channels[i] == *sample) {
			Mix_HaltChannel(i);
			/* Mix_FadeOutChannel(i, ms); */
		}
	}
	return 0;
}

static int Lua_Sound_isPlaying (lua_State *L) {
	Mix_Chunk **sample = checksound(L);
	int playing = 0;
	int i;
	for (i=0; i<CHANNELS; i++) {
		if ((channels[i] == *sample) && Mix_Playing(i)) {
			playing++;
		}
	}
	lua_pushboolean(L, playing);
	return 1;
}

static int Lua_Sound_isPaused (lua_State *L) {
	Mix_Chunk **sample = checksound(L);
	int paused = 0;
	int i;
	for (i=0; i<CHANNELS; i++) {
		if ((channels[i] == *sample) && Mix_Paused(i)) {
			paused++;
		}
	}
	lua_pushboolean(L, paused);
	return 1;
}

static int sound_gc(lua_State *L) {
	/* stack(-1): sound userdata to free */
	Mix_Chunk **ptr = checksound(L);
	Mix_FreeChunk(*ptr);
	/* fprintf(stdout, "Freed sound.\n"); */
	return 0;
}

static int sound_tostring(lua_State *L) {
	lua_pushliteral(L, "Sound");
	return 1;
}

/*******************************************
	Music functions
********************************************/
static int Lua_Music_load(lua_State *L) {
	Mix_Music *music;
	Mix_Music **ptr;
	SDL_RWops *src;
	const char* filename = luaL_checkstring(L, 1);

	src = PHYSFSRWOPS_openRead(filename);
	if (!src) {
		return luaL_error(L, "Error loading music '%s': %s", filename, SDL_GetError());
	}
	music = Mix_LoadMUS_RW(src);
	if (!music) {
		return luaL_error(L, "Error loading music '%s': %s", filename, Mix_GetError());
	}
	/* new userdata for pointer to music */
	ptr = lua_newuserdata(L, sizeof(Mix_Music*));
	*ptr = music;
	luaL_getmetatable(L, "scrupp.music");
	lua_setmetatable(L, -2);
	return 1;
}

static int Lua_Music_play(lua_State *L) {
	Mix_Music **music = checkmusic(L);
	int loops = luaL_optint(L, 2, 0);	/* default: infinite looping */
	int ms = luaL_optint(L, 3, 0);
	luaL_argcheck(L, loops>=0, 2, "number of loops has to be positive");
	luaL_argcheck(L, ms>=0, 3, "Fade-in time has to be positive");
	Mix_HaltMusic();
	if (Mix_FadeInMusic(*music, loops-1, ms) == -1) {
		return luaL_error(L, "Error playing music: %s", Mix_GetError());
	}
	return 0;
}

static int Lua_Music_setVolume(lua_State *L) {
	int volume = luaL_checkint(L, 1);
	luaL_argcheck(L, (volume>=0) && (volume<=MIX_MAX_VOLUME), 1, "0 <= volume <= " xstr(MIX_MAX_VOLUME));
	Mix_VolumeMusic(volume);
	return 0;
}

static int Lua_Music_getVolume(lua_State *L) {
	lua_pushinteger(L, Mix_VolumeMusic(-1));
	return 1;
}

static int Lua_Music_pause (lua_State *L) {
	UNUSED(L);
	Mix_PauseMusic();
	return 0;
}

static int Lua_Music_resume (lua_State *L) {
	UNUSED(L);
	Mix_ResumeMusic();
	return 0;
}

static int Lua_Music_rewind (lua_State *L) {
	UNUSED(L);
	Mix_RewindMusic();
	return 0;
}

static int Lua_Music_stop (lua_State *L) {
	int ms = luaL_optint(L, 1, 0);
	luaL_argcheck(L, ms>=0, 1, "Fade-out time has to be positive");
	Mix_FadeOutMusic(ms);
	return 0;
}

static int Lua_Music_isPlaying (lua_State *L) {
	lua_pushboolean(L, Mix_PlayingMusic());
	return 1;
}

static int Lua_Music_isPaused (lua_State *L) {
	lua_pushboolean(L, Mix_PausedMusic());
	return 1;
}

static int music_gc(lua_State *L) {
	/* stack(-1): music userdata to free */
	Mix_Music **ptr = checkmusic(L);
	Mix_FreeMusic(*ptr);
	/* fprintf(stdout, "Freed music.\n"); */
	return 0;
}

static int music_tostring(lua_State *L) {
	lua_pushliteral(L, "Music");
	return 1;
}

static const struct luaL_Reg soundlib [] = {
	{"addSound",		Lua_Sound_load},
	{"addMusic",		Lua_Music_load},
	{"setMusicVolume",	Lua_Music_setVolume},
	{"getMusicVolume",	Lua_Music_getVolume},
	{"pauseMusic",		Lua_Music_pause},
	{"resumeMusic",		Lua_Music_resume},
	{"rewindMusic",		Lua_Music_rewind},
	{"stopMusic",		Lua_Music_stop},
	{"musicIsPlaying",	Lua_Music_isPlaying},
	{"musicIsPaused",	Lua_Music_isPaused},
	{NULL, NULL}
};

static const struct luaL_Reg soundlib_m [] = {
	{"__gc", 			sound_gc},
	{"__tostring", 		sound_tostring},
	{"play", 			Lua_Sound_play},
	{"pause",			Lua_Sound_pause},
	{"resume",			Lua_Sound_resume},
	{"stop",			Lua_Sound_stop},
	{"isPlaying",		Lua_Sound_isPlaying},
	{"isPaused",		Lua_Sound_isPaused},
	{"setVolume",		Lua_Sound_setVolume},
	{"getVolume",		Lua_Sound_getVolume},
	{NULL, NULL}
};

static const struct luaL_Reg musiclib_m [] = {
	{"__gc",			music_gc},
	{"__tostring",		music_tostring},
	{"play",			Lua_Music_play},
	{NULL, NULL}
};


int luaopen_sound(lua_State *L, const char *parent) {
	if (initSound() == -1) {
		fprintf(stderr,	"Error running Mix_OpenAudio: %s\n"
						"\tSupport of sound and music has been disabled.\n", 
						Mix_GetError());
		return -1;
	}
	luaL_newmetatable(L, "scrupp.sound");
	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);	/* duplicates the metatable */
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, soundlib_m);
	lua_pop(L, 1);	/* pop the metatable */

	luaL_newmetatable(L, "scrupp.music");
	/* metatable.__index = metatable */
	lua_pushvalue(L, -1);	/* duplicates the metatable */
	lua_setfield(L, -2, "__index");
	luaL_register(L, NULL, musiclib_m);
	lua_pop(L, 1);	/* pop the metatable */

	luaL_register(L, parent, soundlib);

	return 1;
}
