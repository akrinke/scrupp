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
#include "Sound.h"
#include "Movie.h"

#include <string.h>
#include <SDL_opengl.h>

lua_State *L;

int done = 0;

/* minimum delta between frames in milliseconds */
Uint32 minimumDelta = 10;

/* Last iteration's tick value */
Uint32 lastTick;

/* checks whether an error generated by Lua is actual generated by scrupp.exit() */
/* this lets us exit the app from within Lua */
int check_for_exit() {
	const char *err_msg;

	err_msg = lua_tostring(L, -1);
	if (strcmp(err_msg, "scrupp.exit") == 0) {
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

/* Lua functions */

static int Lua_Main_setDelta(lua_State *L) {
	minimumDelta = luaL_checkint(L, 1);
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

/* main - function - entry point */
int main(int argc, char *argv[]) {
	SDL_Event event;
	Uint32 lastTick;	/* Last iteration's tick value */
	Uint32 delta = 0;
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

	FS_Init(argc, argv, &filename);	/* initialize virtual filesystem */

	/* register Lua functions */
	lua_newtable(L);
	luaopen_main(L, NULL);
	luaopen_fileio(L, NULL);
	luaopen_font(L, NULL);
	luaopen_graphics(L, NULL);
	luaopen_sound(L, NULL);
	luaopen_mouse(L, NULL);
	luaopen_keyboard(L, NULL);
	luaopen_movie(L, NULL);
	lua_setglobal(L, NAMESPACE);

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
	if ((lua_pcall(L, narg, 0, -(narg+2)) != 0) && !check_for_exit()) {
		error(L, "Error running '%s':\n\t%s", filename, lua_tostring(L, -1));
	}

	/* the error function stays on the stack */

	if (SDL_GetVideoSurface() == NULL)
		error(L, "Error: " PROG_NAME " was not initialized by " NAMESPACE ".init()!\n");

	/* main loop */

	/* Wait until SDL_QUIT event type (window closed) or a call to scrupp.exit() occurs */
	while ( !done ) {
		lastTick = SDL_GetTicks();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		/* maybe the 'main' table has changed
		   so get a new reference in every cycle */
		lua_getglobal(L, "main");		

		if (lua_isnil(L, -1)) {
			error(L, "Error: Table 'main' not found!\n");
		}

		/* at this point, the stack always contains 
		   the error function and the 'main' table */

		/* main.render(delta) */
		lua_getfield(L, -1, "render");
		lua_pushinteger(L, delta);
		if ((lua_pcall(L, 1, 0, -4) != 0) && !check_for_exit()) {
			error(L, "Error running main.render:\n\t%s\n", lua_tostring(L, -1));
		}

		SDL_GL_SwapBuffers();

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
				if ((lua_pcall(L, 1, 0, -4) != 0) && !check_for_exit()) {
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
				if ((lua_pcall(L, 1, 0, -4) != 0) && !check_for_exit()) {
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
				if ((lua_pcall(L, 3, 0, -6) != 0) && !check_for_exit()) {
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
				if ((lua_pcall(L, 3, 0, -6) != 0) && !check_for_exit()) {
					error(L, "Error running main.mousereleased:\n\t%s\n", lua_tostring(L, -1));
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
				if ((lua_pcall(L, 2, 0, -5) != 0) && !check_for_exit()) {
					error(L, "Error running main.resized:\n\t%s\n", lua_tostring(L, -1));
				}
				break;
			}
		}

		lua_pop(L, 1);	/* pop 'main' table */

		delta = SDL_GetTicks() - lastTick;

		/*
		if (delta>1) {
			fprintf(stdout, "delta: %d\n", delta);
		}
		*/

		while (delta < minimumDelta) {
			SDL_Delay(1);
			delta = SDL_GetTicks() - lastTick;
		}

	}
	lua_close(L);
	return 0;
}
