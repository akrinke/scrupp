/*
** $Id$
** Main file
** See Copyright Notice in COPYRIGHT
*/

#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef __unix__
# include <unistd.h>
#elif __MSDOS__ || __WIN32__ || _MSC_VER
# include <direct.h>
# define chdir _chdir
#endif

#include <SDL.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define UNUSED(x) ((void)(x)) /* to avoid warnings */

#define PROG_NAME 	"Scrupp"
#ifndef VERSION
# define VERSION "0.4"
#endif
#define COPYRIGHT	"Copyright (C) 2005-2009 Andreas Krinke"
#define AUTHOR		"Andreas Krinke"

#define NAMESPACE	"scrupp"

#define DEFAULT_FILE	"main.slua"
#define DEFAULT_ARCHIVE	"main.sar"

#define SCRUPP_PATH				"SCRUPP_PATH"
#define SCRUPP_PATH_DEFAULT		"?.lua;scripts/?.lua;scripts/?/init.lua;scripts/socket/?.lua"

#define CHANNELS	16	/* number of audio channels */

int check_for_exit(lua_State *L);
int error_function(lua_State *L);

#endif /*__MAIN_H__*/
