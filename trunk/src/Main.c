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

#include <string.h>
#include <SDL_opengl.h>

lua_State *L;

int done = 0;

/* Last iteration's tick value */
Uint32 lastTick;

/* checks whether an error generated by Lua is actual generated by game.exit() */
/* this lets us exit the app from within Lua */
int check_for_exit() {
	const char *err_msg;

	err_msg = lua_tostring(L, -1);
	if (strcmp(err_msg, "game.exit") == 0) {
		done = 1;
		fprintf(stdout, "Exiting game.\n");
		lua_close(L);
		exit(0);
		return 1;
	} else
		return 0;
}

static void usage(const char* exec_name) {
	fprintf(stdout, "usage:\n\t%s [archive] [arguments]\nor\n\t%s -e [Lua file] [arguments]\n", exec_name, exec_name);
	exit(1);
}

int error_function(lua_State *L) {
	const char *err_msg;
	char *pos;
	size_t len;
	
	err_msg = lua_tolstring(L, -1, &len); /* get error message */
	pos = strstr(err_msg, "game.exit");
	if (pos && (len-(pos-err_msg) == 9)) { /* ends the error message with "game.exit"? */
		lua_pushliteral(L, "game.exit");
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

static int Lua_Main_exit(lua_State *L) {
	/* game.exit is implemented by throwing an error with a special message */
	/* the error handler checks for this message using check_for_exit() */
	return luaL_error(L, "game.exit");
}

static const struct luaL_Reg mainlib [] = {
	{"exit", Lua_Main_exit},
	{NULL, NULL}
};

static int luaopen_main(lua_State* L, const char *parent) {
	luaL_register(L, parent, mainlib);	/* leaves table on top of stack */
	lua_pushliteral(L, "_VERSION");
	lua_pushliteral(L, VERSION);
	lua_rawset(L, -3);
	return 1;
}

/* main - function - entry point */
int main(int argc, char *argv[]) {
	SDL_Event event;
	Uint32 lastTick;	/* Last iteration's tick value */
	Uint32 delta = 0;
	int i, n, narg, nres, result;
	int e_flag = 0;
	char *filename = NULL;

	fprintf(stdout, "%s v%s\n", PROG_NAME, VERSION);

	for (n=1; argv[n] != NULL; n++) {
		if (argv[n][0] == '-') {
			if ((argv[n][1] == 'e') && (argv[n][2] == '\0')) {
				e_flag = 1;
				filename = argv[n+1];
				if (filename == NULL)
					usage(argv[0]);
			} else
				usage(argv[0]);
		} else
			break;
	}
	if (filename == NULL) filename = "main.lua";
	if (argc == 1) n = 0;	/* no arguments given */

	L = luaL_newstate();	/* initialize Lua */

	FS_Init(argc, argv, e_flag, &filename);	/* initialize Filesystem */

	luaL_openlibs(L);	/* load Lua base libraries */

	/* register Lua functions */
	lua_newtable(L);
	luaopen_main(L, NULL);
	luaopen_fileio(L, NULL);
	luaopen_font(L, NULL);
	luaopen_graphics(L, NULL);
	luaopen_sound(L, NULL);
	lua_setglobal(L, "game");

	luaopen_mouse(L, "mouse");
	luaopen_keyboard(L, "key");
	lua_pop(L, 2); /* pop "mouse" and "key" tables */
	
	/* create arg-table for the command-line-arguments */
	/* taken from lua.c of Lua */
	narg = argc - (n+1); /* number of arguments to the script */
	luaL_checkstack(L, narg + 3, "too many arguments to script");
	for (i=n+1; i<argc; i++)
		lua_pushstring(L, argv[i]);
	lua_createtable(L, narg, n+1);
	for (i=0; i < argc; i++) {
		lua_pushstring(L, argv[i]);
		lua_rawseti(L, -2, i - n);
	}
	lua_setglobal(L, "arg");
	
	/* run main lua file */
	/* if two arguments are given (then the first has to be '-e'), */
	/* the second one will be executed as Lua file */
	result = FS_runLuaFile(filename, narg, &nres);
	if ((result == ERROR) && !check_for_exit())
		error(L, lua_tostring(L, -1));

	/* main loop */

	/* Wait until SDL_QUIT event type (window closed) or a call to game.exit() occurs */
	while ( !done ) {
		lastTick = SDL_GetTicks();

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glLoadIdentity();

		lua_getglobal(L, "main");

		if (lua_isnil(L, -1))
			error(L, "Error: Table 'main' not found!\n");

		/* main.render(delta) */
		lua_getfield(L, -1, "render");
		if (!lua_isfunction(L, -1))
			error(L, "Error: main.render is not defined or no function!\n");
		lua_pushinteger(L, delta);
		lua_pushcfunction(L, error_function);
		lua_insert(L, -3);
		if ((lua_pcall(L, 1, 0, -3) != 0) && !check_for_exit())
			error(L, "Error running main.render:\n\t%s\n", lua_tostring(L, -1));
		lua_pop(L, 1); /* remove error_function */

		SDL_GL_SwapBuffers();

		while ( SDL_PollEvent( &event ) ) {
			switch ( event.type ) {
			case SDL_QUIT:
				done = 1;
				break;

			case SDL_KEYDOWN:
				lua_getfield(L, -1, "keypressed");
				if (!lua_isfunction(L, -1)) {
					lua_pop(L, 1); /* pop if it's no a function */
					break;
				}
				lua_pushinteger(L, event.key.keysym.sym);
				lua_pushcfunction(L, error_function);
				lua_insert(L, -3);
				if ((lua_pcall(L, 1, 0, -3) != 0) && !check_for_exit())
					error(L, "Error running main.keypressed:\n\t%s\n", lua_tostring(L, -1));
				lua_pop(L, 1); /* remove error_function */
				break;

			case SDL_KEYUP:
				lua_getfield(L, -1, "keyreleased");
				if (!lua_isfunction(L, -1)) {
					lua_pop(L, 1); /* pop if it's no a function */
					break;
				}
				lua_pushinteger(L, event.key.keysym.sym);
				lua_pushcfunction(L, error_function);
				lua_insert(L, -3);
				if ((lua_pcall(L, 1, 0, -3) != 0) && !check_for_exit())
					error(L, "Error running main.keyreleased:\n\t%s\n", lua_tostring(L, -1));
				lua_pop(L, 1); /* remove error_function */
				break;

			case SDL_MOUSEBUTTONDOWN:
				lua_getfield(L, -1, "mousepressed");
				if (!lua_isfunction(L, -1)) {
					lua_pop(L, 1); /* pop if it's no a function */
					break;
				}
				lua_pushinteger(L, event.button.x);
				lua_pushinteger(L, event.button.y);
				lua_pushstring(L, buttonNames[event.button.button-1]);
				lua_pushcfunction(L, error_function);
				lua_insert(L, -5);
				if ((lua_pcall(L, 3, 0, -5) != 0) && !check_for_exit())
					error(L, "Error running main.mousepressed:\n\t%s\n", lua_tostring(L, -1));
				lua_pop(L, 1); /* remove error_function */
				break;

			case SDL_MOUSEBUTTONUP:
				lua_getfield(L, -1, "mousereleased");
				if (!lua_isfunction(L, -1)) {
					lua_pop(L, 1); /* pop if it's no a function */
					break;
				}
				lua_pushinteger(L, event.button.x);
				lua_pushinteger(L, event.button.y);
				lua_pushstring(L, buttonNames[event.button.button-1]);
				lua_pushcfunction(L, error_function);
				lua_insert(L, -5);
				if ((lua_pcall(L, 3, 0, -5) != 0) && !check_for_exit())
					error(L, "Error running main.mousereleased:\n\t%s\n", lua_tostring(L, -1));
				lua_pop(L, 1); /* remove error_function */
				break;
			}
		}

		lua_pop(L, 1);	// pop 'main' table

		delta = SDL_GetTicks() - lastTick;

		/*
		if (delta>1)
			fprintf(stdout, "delta: %d\n", delta);
		*/

		if (delta < 20) {
			SDL_Delay(20 - delta);
			delta = SDL_GetTicks() - lastTick;
		}

	}
	lua_close(L);
	return 0;
}