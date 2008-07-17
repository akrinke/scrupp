/*
** $Id$
** File I/O: Access to the virtual file system
** See Copyright Notice in COPYRIGHT
*/

#ifndef __FILEIO_H__
#define __FILEIO_H__

void FS_Init(int argc, char *argv[], char **filename);
int FS_runLuaFile(const char *filename, int narg, int *nres);

int luaopen_fileio(lua_State *L, const char *parent);

#endif /* __FILEIO_H__ */
