/*
** $Id$
** Functions for general use
** See Copyright Notice in COPYRIGHT
*/

#ifndef __MACROS_H__
#define __MACROS_H__

/* Macros to stringify the result of expansion of a macro argument */
#define xstr(s) str(s)
#define str(s) #s

void error (lua_State *L, const char *fmt, ...);

/* Dumps the stack - DEBUG function */
void stackDump (lua_State *L);

int getint(lua_State *L, int *result, int key);
int getdouble(lua_State *L, double *result, int key);

#endif /* __MACROS_H__ */
