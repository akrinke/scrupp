/*
** $Id$
** Functions for general use
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"

void error (lua_State *L, const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  vfprintf(stderr, fmt, argp);
  va_end(argp);
  lua_close(L);
  exit(1);
}

/* Dumps the stack - DEBUG function */
void stackDump (lua_State *L) {
	int i, t;
	int top = lua_gettop(L);
	
	printf("stackDump: ");
	for (i = 1; i <= top; i++) {  /* repeat for each level */
	t = lua_type(L, i);
	switch (t) {
		case LUA_TSTRING:  /* strings */
			printf("`%s'", lua_tostring(L, i));
			break;

		case LUA_TBOOLEAN:  /* booleans */
			printf(lua_toboolean(L, i) ? "true" : "false");
			break;

		case LUA_TNUMBER:  /* numbers */
			printf("%g", lua_tonumber(L, i));
			break;

		default:  /* other values */
			printf("%s", lua_typename(L, t));
			break;
		}
		printf("  ");  /* put a separator */
	}
	printf("\n");  /* end the listing */
}

int getint(lua_State *L, int *result, int key) {
	lua_pushinteger(L, key);
	lua_gettable(L, -2);
	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);
		return 0;
	}
	*result = (int)lua_tonumber(L, -1);
	lua_pop(L, 1);
	return 1;
}
