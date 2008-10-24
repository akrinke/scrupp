/*
** $Id$
** Functions for general use
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"

#ifdef WIN32
#include <windows.h>
#define vsnprintf _vsnprintf
#endif

#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

void error (lua_State *L, const char *fmt, ...) {
	char str[1024];
	const char *msg;
	
	va_list argp;
	va_start(argp, fmt);
	vsnprintf(str, 1023, fmt, argp);
	va_end(argp);
	
	msg = luaL_gsub(L, str, "[\"", "'");
	msg = luaL_gsub(L, msg, "[string \"", "'");
	msg = luaL_gsub(L, msg, "\"]", "'");
	fprintf(stderr, msg);

#ifdef WIN32
	MessageBox(NULL, msg, PROG_NAME, MB_ICONERROR|MB_OK);
#endif

#ifdef __APPLE__
	CFUserNotificationDisplayAlert (
		0, /* timeout */ 
		kCFUserNotificationStopAlertLevel, /* alert level */
		NULL, /* icon URL */ 
		NULL, /* sound URL */
		NULL, /* localization URL */
		CFSTR(PROG_NAME), /* header */
		CFStringCreateWithCString(
			NULL, /* choose default allocator */
			msg,
			kCFStringEncodingASCII /* encoding */			
		), 
		NULL, /* default button title (OK) */
		NULL, /* alternate button title */
		NULL, /* other button title */
		NULL /* response codes */
	);
#endif
	
	fprintf(stderr, "\n");
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
