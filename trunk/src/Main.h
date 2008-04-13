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
#include <unistd.h>

#include <SDL.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define PROG_NAME 	"Scrupp"
#define VERSION 	"0.1"
#define COPYRIGHT	"Copyright (C) 2005-2008 Andreas Krinke"
#define AUTHOR		"Andreas Krinke"

#define CHANNELS	16	/* number of audio channels */

SDL_Surface		*screen;
lua_State* 		L;

int check_for_exit();

#endif /*__MAIN_H__*/
