/*
** $Id$
** Functions for general use
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"
#include "Macros.h"

#ifdef USE_GTK
	#include <gtk/gtk.h>
#elif __WIN32__
	#include <windows.h>
	#define vsnprintf _vsnprintf
#elif __MACOSX__
	#include <Carbon/Carbon.h>
#endif

void error (lua_State *L, const char *fmt, ...) {
	char str[1024];
	const char *msg;
#ifdef USE_GTK
	GtkWidget *dialog;
#endif
	
	va_list argp;
	va_start(argp, fmt);
	vsnprintf(str, 1023, fmt, argp);
	va_end(argp);

	/* A fullscreen window might hide the error box,
	 * therefore, we kill SDL first. */
	SDL_Quit();
	
	msg = luaL_gsub(L, str, "[\"", "'");
	msg = luaL_gsub(L, msg, "[string \"", "'");
	msg = luaL_gsub(L, msg, "\"]", "'");
	fputs(msg, stderr);
	fputs("\n", stderr);

#ifdef USE_GTK
	gtk_init(NULL, NULL);
	dialog = gtk_message_dialog_new(
		NULL,
		GTK_DIALOG_MODAL,
		GTK_MESSAGE_ERROR,
		GTK_BUTTONS_CLOSE,
		"%s", msg
	);
	gtk_window_set_title(GTK_WINDOW (dialog), PROG_NAME);
	gtk_dialog_run(GTK_DIALOG (dialog));

#elif __WIN32__
	MessageBox(NULL, msg, PROG_NAME, MB_ICONERROR|MB_OK);

#elif __MACOSX__
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

	lua_close(L);
	exit(1);
}

/* Dumps the stack - DEBUG function */
void stackDump (lua_State *L) {
	int i, t;
	int top = lua_gettop(L);
	
	puts("stackDump: ");
	for (i = 1; i <= top; i++) {  /* repeat for each level */
	t = lua_type(L, i);
	switch (t) {
		case LUA_TSTRING:  /* strings */
			printf("`%s'", lua_tostring(L, i));
			break;

		case LUA_TBOOLEAN:  /* booleans */
			if (lua_toboolean(L, i)) {
				puts("true");
			} else {
				puts("false");
			}
			break;

		case LUA_TNUMBER:  /* numbers */
			printf("%g", lua_tonumber(L, i));
			break;

		default:  /* other values */
			puts(lua_typename(L, t));
			break;
		}
		puts("  ");  /* put a separator */
	}
	puts("\n");  /* end the listing */
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

int getdouble(lua_State *L, double *result, int key) {
	lua_pushinteger(L, key);
	lua_gettable(L, -2);
	if (!lua_isnumber(L, -1)) {
		lua_pop(L, 1);
		return 0;
	}
	*result = lua_tonumber(L, -1);
	lua_pop(L, 1);
	return 1;
}
