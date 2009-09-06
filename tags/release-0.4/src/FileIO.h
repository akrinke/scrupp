/*
** $Id$
** File I/O: Access to the virtual file system
** See Copyright Notice in COPYRIGHT
*/

#ifndef __FILEIO_H__
#define __FILEIO_H__

#define FILEIO_SUCCESS 0
#define FILEIO_ERROR 1

void FS_Init(lua_State *L, char *argv[], char **filename);
int FS_loadFile(lua_State *L, const char *filename);

int luaopen_fileio(lua_State *L, const char *parent);

#endif /* __FILEIO_H__ */
