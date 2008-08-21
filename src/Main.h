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

#define PROG_NAME 	"Scrupp"
#define VERSION 	"0.1"
#define COPYRIGHT	"Copyright (C) 2005-2008 Andreas Krinke"
#define AUTHOR		"Andreas Krinke"

#define NAMESPACE	"scrupp"

#define DEFAULT_FILE	"main.slua"
#define DEFAULT_ARCHIVE	"main.sar"

#ifndef SHARE_DIR
#define SHARE_DIR		"/usr/share/scrupp/"
#endif

#define CHANNELS	16	/* number of audio channels */

#define TRUE    0
#define FALSE   1
#define SUCCESS 0
#define ERROR   1

extern lua_State* 		L;

int check_for_exit();
int error_function(lua_State *L);

#endif /*__MAIN_H__*/
