/*
** $Id$
** Main file
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"

#include "Macros.h"
#include "FileIO.h"
#include "Font.h"
#include "Graphics.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Joystick.h"
#include "Sound.h"
#include "Movie.h"

/* luafilesystem */
#include "luafilesystem/lfs.h"

/* luagl and luaglu */
#include "luagl/luagl.h"
#include "luagl/luaglu.h"

/* luasocket */
#include "luasocket/luasocket.h"
#include "luasocket/mime.h"

/* chipmunk binding werechip */
#include "werechipmunk/werechipmunk.h"

#include <string.h>
#include <SDL_opengl.h>

#ifdef __WIN32__
#include <windows.h>
#else
#include <time.h>
#include <sys/time.h>
#endif

int done = 0;

/* minimum delta between frames in milliseconds */
double minimumDelta = 30.0;

/* checks whether an error generated by Lua is actual generated by scrupp.exit() */
/* this lets us exit the app from within Lua */
int check_for_exit(lua_State *L) {
	const char *err_msg;
	size_t len;

	err_msg = lua_tolstring(L, -1, &len);
	if (len > 0 && strcmp(err_msg, "scrupp.exit") == 0) {
		done = 1;
		fprintf(stdout, "Exiting Scrupp.\n");
		lua_close(L);
		exit(0);
		return 1;
	} else {
		return 0;
	}
}
/*
static void usage(const char* exec_name) {
	fprintf(stdout, "usage:\n\t%s [Lua file] [arguments]\nor\n\t%s [archive] [arguments]\nor\n\t%s [directory] [arguments]", exec_name, exec_name, exec_name);
	exit(1);
}
*/
int error_function(lua_State *L) {
	const char *err_msg;
	char *pos;
	size_t len;

	err_msg = lua_tolstring(L, -1, &len); /* get error message */
	if (len == 0) {
		return 1;
	}

	pos = strstr(err_msg, "scrupp.exit");
	if (pos && (len-(pos-err_msg) == 11)) { /* ends the error message with "scrupp.exit"? */
		lua_pushliteral(L, "scrupp.exit");
		return 1;
	}

	luaL_gsub(L, err_msg, ": ", ":\n\t");
	lua_pushliteral(L, "\n");
	lua_concat(L, 2);

	lua_getfield(L, LUA_GLOBALSINDEX, "debug");
	if (!lua_istable(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_getfield(L, -1, "traceback");
	if (!lua_isfunction(L, -1)) {
		lua_pop(L, 1);
		return 1;
	}
	lua_pushvalue(L, -3); /* push error message */

	lua_pushinteger(L, 2); /* start at level 2 */
	lua_call(L, 2, 1);

	return 1;
}

/* copied from LuaSocket */
#ifdef __WIN32__
static inline double get_time(void) {
    FILETIME ft;
    double t;
    GetSystemTimeAsFileTime(&ft);
    /* Windows file time (time since January 1, 1601 (UTC)) */
    t  = ft.dwLowDateTime/1.0e7 + ft.dwHighDateTime*(4294967296.0/1.0e7);
    /* convert to Unix Epoch time (time since January 1, 1970 (UTC)) */
    return ((t - 11644473600.0)*1.0e3);
}
#else
static inline double get_time(void) {
    struct timeval v;
    gettimeofday(&v, (struct timezone *) NULL);
    /* Unix Epoch time (time since January 1, 1970 (UTC)) */
    return v.tv_sec*1.0e3 + v.tv_usec/1.0e3;
}
#endif

/* Lua functions */

static int Lua_Main_setDelta(lua_State *L) {
	minimumDelta = luaL_checknumber(L, 1);
	return 0;
}

static int Lua_Main_exit(lua_State *L) {
	/* scrupp.exit is implemented by throwing an error with a special message */
	/* the error handler checks for this message using check_for_exit() */
	return luaL_error(L, "scrupp.exit");
}

static const struct luaL_Reg mainlib [] = {
	{"setDelta", Lua_Main_setDelta},
	{"exit", Lua_Main_exit},
	{NULL, NULL}
};

static int luaopen_main(lua_State* L, const char *parent) {
	luaL_register(L, parent, mainlib);	/* leaves table on top of stack */
	lua_pushliteral(L, "VERSION");
	lua_pushliteral(L, VERSION);
	lua_rawset(L, -3);
	lua_pushliteral(L, "PLATFORM");
#ifdef __WIN32__
	lua_pushliteral(L, "Windows");
#elif __MACOSX__
	lua_pushliteral(L, "Mac OS X");
#elif __LINUX__
	lua_pushliteral(L, "Linux");
#else
	lua_pushliteral(L, "Unknown");
#endif
	lua_rawset(L, -3);
	return 1;
}

/* main-function - entry point */
int main(int argc, char *argv[]) {
	lua_State *L;
	SDL_Event event;
	Uint16 wchar;
	char buf[4] = {0};
	int redraw = 0;		/* redraw is needed or not */
	double lastTime;	/* last iteration's tick value */
	double delta = 0.0;
	int i, n, narg;
	char *filename = NULL;
	fprintf(stdout, "%s v%s\n", PROG_NAME, VERSION);

	if (argc > 1) {
		n = 1;
		if (strcmp(argv[1], "--") != 0)
			filename = argv[1];			
	} else {
		n = argc - 1;
	}

	L = luaL_newstate();	/* initialize Lua */
	luaL_openlibs(L);	/* load Lua base libraries */

	FS_Init(L, argv, &filename);	/* initialize virtual filesystem */

	/* register Lua functions */
	lua_newtable(L);
	luaopen_main(L, NULL);
	luaopen_fileio(L, NULL);
	luaopen_font(L, NULL);
	luaopen_graphics(L, NULL);
	luaopen_sound(L, NULL);
	luaopen_mouse(L, NULL);
	luaopen_keyboard(L, NULL);
	luaopen_joystick(L, NULL);
	luaopen_movie(L, NULL);
	lua_setglobal(L, NAMESPACE);
	
	/* flush the buffer of stdout */
	fflush(stdout);
	
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");

	/* put luaopen_lfs in package.preload["lfs"] */
	lua_pushcfunction(L, luaopen_lfs);
	lua_setfield(L, -2, "lfs");

	/* put luaopen_luagl in package.preload["luagl"] */
	lua_pushcfunction(L, luaopen_luagl);
	lua_setfield(L, -2, "luagl");

	/* put luaopen_luaglu in package.preload["luaglu"] */
	lua_pushcfunction(L, luaopen_luaglu);
	lua_setfield(L, -2, "luaglu");

	/* put wrapper for luaopen_oocairo in package.preload["oocairo"] */
	lua_pushcfunction(L, luaopen_oocairo_wrapper);
	lua_setfield(L, -2, "oocairo");
	
	/* put luaopen_socket_core in package.preload["socket.core"] */
	lua_pushcfunction(L, luaopen_socket_core);
	lua_setfield(L, -2, "socket.core");
	
	/* put luaopen_mime_core in package.preload["mime.core"] */
	lua_pushcfunction(L, luaopen_mime_core);
	lua_setfield(L, -2, "mime.core");
	
	/* put luaopen_werechip in package.preload["werechip"] */
	lua_pushcfunction(L, luaopen_werechip);
	lua_setfield(L, -2, "werechip");
	
	lua_pop(L, 2);	
	
	/* push the error function for the protected calls later on */
	lua_pushcfunction(L, error_function);

	/* load and compile main lua file */
	if (FS_loadFile(L, filename) == FILEIO_ERROR) {
		error(L, "Error loading '%s':\n\t%s", filename, lua_tostring(L, -1));
	}

	/* push all script arguments */
	/* create arg-table for the command-line-arguments */
	/* copied from lua.c of the Lua distribution */
	narg = argc - (n+1); /* number of arguments to the script */
	luaL_checkstack(L, narg+2, "too many arguments to script");
	for (i=n+1; i<argc; i++) {
		lua_pushstring(L, argv[i]);
	}
	lua_createtable(L, narg, n+1);
	for (i=0; i<argc; i++) {
		lua_pushstring(L, argv[i]);
		lua_rawseti(L, -2, i - n);
	}
	lua_setglobal(L, "arg");

	/* run the compiled chunk */
	if ((lua_pcall(L, narg, 0, -(narg+2)) != 0) && !check_for_exit(L)) {
		error(L, "Error running '%s':\n\t%s", filename, lua_tostring(L, -1));
	}

	/* the error function stays on the stack */

	if (SDL_GetVideoSurface() == NULL)
		error(L, "Error: " PROG_NAME " was not initialized by " NAMESPACE ".init()!\n");

	/* do a glClear, because it will always be called after SDL_GL_SwapBuffers */
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	/* main loop */

	/* Wait until SDL_QUIT event type (window closed) or a call to scrupp.exit() occurs */
	while ( !done ) {
		lastTime = get_time();

		/* maybe the 'main' table has changed
		   so get a new reference in every cycle */
		lua_getglobal(L, "main");

		if (lua_isnil(L, -1)) {
			error(L, "Error: Table 'main' not found!\n");
		}

		/* at this point, the stack always contains 
		   the error function and the 'main' table */

		/* main.update(delta) */
		lua_getfield(L, -1, "update");
		if (!lua_isnil(L, -1)) {
			lua_pushnumber(L, delta);
			if ((lua_pcall(L, 1, 1, -4) != 0) && !check_for_exit(L)) {
				error(L, "Error running main.update:\n\t%s\n", lua_tostring(L, -1));
			}
		}
		
		/* if update function exists, return value is on top of the stack, otherwise nil */
		
		if (lua_toboolean(L, -1) == 0 || redraw) {
			redraw = 0;
			/* return value is false or nil -> call main.render because frame is not skipped */
			/* main.render(delta) */
			lua_getfield(L, -2, "render");
			lua_pushnumber(L, delta);
			if ((lua_pcall(L, 1, 0, -5) != 0) && !check_for_exit(L)) {
				error(L, "Error running main.render:\n\t%s\n", lua_tostring(L, -1));
			}
			SDL_GL_SwapBuffers();
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		}
		lua_pop(L, 1);

		while ( SDL_PollEvent( &event ) ) {
			switch ( event.type ) {
			case SDL_QUIT:
				done = 1;
				break;

			case SDL_KEYDOWN:
				lua_getfield(L, -1, "keypressed");
				if (lua_isnil(L, -1)) {
					lua_pop(L, 1); /* pop if it's nil */
					break;
				}
				lua_pushliteral(L, "Scrupp:key_table");
				lua_rawget(L, LUA_REGISTRYINDEX);
				lua_rawgeti(L, -1, event.key.keysym.sym);
				lua_remove(L, -2); /* remove the key_table */
				buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0;
				if (SDL_EnableUNICODE(-1) == 1) {
					/* get the utf-16 code */
					wchar = event.key.keysym.unicode;
					printf("wchar: %d\n", wchar);
					/* convert utf-16 to utf-8 */
					if (wchar < 0x80) {
						buf[0] = wchar;
					} else if (wchar < 0x800) {
						buf[0] = (0xC0 | wchar >> 6);
						buf[1] = (0x80 | wchar & 0x3F);
					} else {
						buf[0] = (0xE0 | wchar >> 12);
						buf[1] = (0x80 | wchar >> 6 & 0x3F);
						buf[2] = (0x80 | wchar & 0x3F);
					}
				}
				lua_pushstring(L, &buf[0]);
				if ((lua_pcall(L, 2, 0, -5) != 0) && !check_for_exit(L)) {
					error(L, "Error running main.keypressed:\n\t%s\n", lua_tostring(L, -1));
				}
				break;

			case SDL_KEYUP:
				lua_getfield(L, -1, "keyreleased");
				if (lua_isnil(L, -1)) {
					lua_pop(L, 1); /* pop if it's nil */
					break;
				}
				lua_pushliteral(L, "Scrupp:key_table");
				lua_rawget(L, LUA_REGISTRYINDEX);
				lua_rawgeti(L, -1, event.key.keysym.sym);
				lua_remove(L, -2); /* remove the key_table */				
				if ((lua_pcall(L, 1, 0, -4) != 0) && !check_for_exit(L)) {
					error(L, "Error running main.keyreleased:\n\t%s\n", lua_tostring(L, -1));
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				lua_getfield(L, -1, "mousepressed");
				if (lua_isnil(L, -1)) {
					lua_pop(L, 1); /* pop if it's nil */
					break;
				}
				lua_pushinteger(L, event.button.x);
				lua_pushinteger(L, event.button.y);
				lua_pushstring(L, buttonNames[event.button.button-1]);
				if ((lua_pcall(L, 3, 0, -6) != 0) && !check_for_exit(L)) {
					error(L, "Error running main.mousepressed:\n\t%s\n", lua_tostring(L, -1));
				}
				break;

			case SDL_MOUSEBUTTONUP:
				lua_getfield(L, -1, "mousereleased");
				if (lua_isnil(L, -1)) {
					lua_pop(L, 1); /* pop if it's nil */
					break;
				}
				lua_pushinteger(L, event.button.x);
				lua_pushinteger(L, event.button.y);
				lua_pushstring(L, buttonNames[event.button.button-1]);
				if ((lua_pcall(L, 3, 0, -6) != 0) && !check_for_exit(L)) {
					error(L, "Error running main.mousereleased:\n\t%s\n", lua_tostring(L, -1));
				}
				break;
			
			case SDL_JOYBUTTONDOWN:
				lua_getfield(L, -1, "joystickpressed");
				if (lua_isnil(L, -1)) {
					lua_pop(L, 1); /* pop if it's nil */
					break;
				}
				lua_pushliteral(L, "Scrupp:joystick_table");
				lua_rawget(L, LUA_REGISTRYINDEX);
				lua_rawgeti(L, -1, event.jbutton.which);
				lua_remove(L, -2); /* remove the joystick_table */
				lua_pushinteger(L, event.jbutton.button+1);
				if ((lua_pcall(L, 2, 0, -5) != 0) && !check_for_exit(L)) {
					error(L, "Error running main.joystickpressed:\n\t%s\n", lua_tostring(L, -1));
				}
				break;
			
			case SDL_JOYBUTTONUP:
				lua_getfield(L, -1, "joystickreleased");
				if (lua_isnil(L, -1)) {
					lua_pop(L, 1); /* pop if it's nil */
					break;
				}
				lua_pushliteral(L, "Scrupp:joystick_table");
				lua_rawget(L, LUA_REGISTRYINDEX);
				lua_rawgeti(L, -1, event.jbutton.which);
				lua_remove(L, -2); /* remove the joystick_table */
				lua_pushinteger(L, event.jbutton.button+1);
				if ((lua_pcall(L, 2, 0, -5) != 0) && !check_for_exit(L)) {
					error(L, "Error running main.joystickreleased:\n\t%s\n", lua_tostring(L, -1));
				}
				break;
				
			case SDL_VIDEORESIZE:
				lua_getfield(L, -1, "resized");
				if (lua_isnil(L, -1)) {
					lua_pop(L, 1); /* pop if it's nil */
					break;
				}
				lua_pushinteger(L, event.resize.w);
				lua_pushinteger(L, event.resize.h);
				if ((lua_pcall(L, 2, 0, -5) != 0) && !check_for_exit(L)) {
					error(L, "Error running main.resized:\n\t%s\n", lua_tostring(L, -1));
				}
				break;
			
			case SDL_VIDEOEXPOSE:
				redraw = 1;
				break;
			}
				
		}

		lua_pop(L, 1);	/* pop 'main' table */

		delta = get_time() - lastTime;

		/*
		if (delta>1) {
			fprintf(stdout, "delta: %d\n", delta);
		}
		*/

		while (delta < minimumDelta) {
			SDL_Delay(1);
			delta = get_time() - lastTime;
		}

	}
	lua_close(L);
	return 0;
}
