/*
** $Id$
** File I/O: Access to the virtual file system
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"
#include "Macros.h"
#include "FileIO.h"

#include <string.h>
#include <stdlib.h>
#include <physfs.h>

/* extracts the dirname and the basename from the path */
static void splitPath(const char *path, char **dirname, char **basename) {
	char *ptr;
	const char *dirsep = NULL;
	*basename = (char *)path;
	*dirname = ".";
	dirsep = PHYSFS_getDirSeparator();
	if (strlen(dirsep) == 1)  /* fast path. */
		ptr = strrchr(path, *dirsep);
	else {
		ptr = strstr(path, dirsep);
		if (ptr != NULL) {
			char *p = ptr;
			while (p != NULL) {
				ptr = p;
				p = strstr(p + 1, dirsep);
			}
		}
	}
	if (ptr != NULL) {
		size_t size = (size_t) (ptr - path);
		*dirname = (char *) malloc(size + 1);
		memcpy(*dirname, path, size);
		(*dirname)[size] = '\0';
		*basename = ptr + strlen(dirsep);
	}
}

static void FS_Quit(void) {
	fprintf(stdout,"Running FS_Quit.\n");
	PHYSFS_deinit();
}

void FS_Init(int argc, char *argv[], int e_flag, char **filename) {
	#ifdef __APPLE__
		char *ch = NULL;
	#endif
	char *dir = NULL;
	char *base = NULL;
	char ** arr = NULL;
	/* initialize PhysFS */
	if ( !PHYSFS_init(argv[0]) ) 
		error(L, "Error: Couldn't initialize PhysFS: %s.", PHYSFS_getLastError());
	
	/*	on Mac OS X applications are packed inside a folder with the ending .app;
		the first thing is to (try to) set the search path to this folder */
	#ifdef __APPLE__
		ch = strstr(argv[0], "/Contents/MacOS");
		if (ch != NULL) {
			*(ch+1) = '\0';
			if ( !PHYSFS_addToSearchPath(argv[0], 1) )
				error(L, "Error: Could not add application folder to search path: %s.", PHYSFS_getLastError() );
			*(ch+1) = 'C';
		}
	#endif
	/* in every case: append base dir to search path */
	if ( !PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1) )
		error(L, "Error: Could not add base dir to search path: %s.", PHYSFS_getLastError());
	
	/* if an archive (or directory) is given, try to mount it */
	if (!e_flag && argv[1] != NULL) {
		/* try to change the working directory (only successful if directory is given) */
		if (chdir(argv[1]) == 0) {
			/* prepend the new working directory to the search path */
			if (!PHYSFS_addToSearchPath(".", 0))
				error(L, "Error: Could not add directory '%s' to search path: %s.", argv[1], PHYSFS_getLastError());
		} else {
			/* chdir was unsuccessful -> archive was probably given on command line */
			splitPath(argv[1], &dir, &base);
			/* change the working directory to the directory with the archive */
			chdir(dir);
			if (!PHYSFS_addToSearchPath(base, 0))
				error(L, "Error: Could not add archive '%s' to search path: %s.", argv[1], PHYSFS_getLastError());
		}
	/* if a filname to execute is given, add its directory to the search path */
	} else if (e_flag && *filename != NULL) {
		splitPath(*filename, &dir, &base);
		/* change the working directory to the directory with the Lua file */
		chdir(dir);
		/* prepend the new working directory to the search path */
		if (!PHYSFS_addToSearchPath(".", 0))
			error(L, "Error: Could not add directory containing '%s' to search path: %s.", base, PHYSFS_getLastError());
		/* change the filename to its basename -> later call to FS_runLuaFile will find it in the path */
		*filename = base;
	}
	
	/* allow symlinks */
	PHYSFS_permitSymbolicLinks(1);
	
	atexit(FS_Quit);

	arr = PHYSFS_getSearchPath();
	while (*arr != NULL)
		printf("search path: %s\n", *arr++);

}

int FS_runLuaFile(const char *filename, int narg, int *nres) {
	char *buffer;		/* buffer for the file */
	char *entryPoint;	/* entry point of file (differs from buffer, if "#!" in the first line is skipped */
	int n;
	int err;
	PHYSFS_file *Hndfile = NULL;
	PHYSFS_sint64 fileLength, size;
	if (PHYSFS_exists(filename) == 0) {
		lua_pushfstring(L, "Error executing '%s': file not found.", filename);
		return ERROR;
	}

	fprintf(stdout, "Executing \"%s\"...\n", filename);
	Hndfile = PHYSFS_openRead(filename); /* open file to read! */
	if (Hndfile == NULL) {
		lua_pushfstring(L, "Error while reading from '%s': %s", filename, PHYSFS_getLastError());
		return ERROR;
	}

	size = PHYSFS_fileLength(Hndfile);
	if (size == -1) {
		lua_pushfstring(L, "Error while determining the size of %s.", filename);
		return ERROR;
	}

	buffer = (char *)malloc((unsigned int)size);
	if (buffer == NULL) {
		lua_pushfstring(L, "Error loading %s: Insufficient memory available.", filename);
		return ERROR;
	}

	fileLength = PHYSFS_read(Hndfile, buffer, 1, (unsigned int)size);
	if (fileLength < size) {
		free(buffer);
		lua_pushfstring(L, "Error while reading from %s: %s", filename, PHYSFS_getLastError());
		return ERROR;
	}
	/* close the file */
	err = PHYSFS_close(Hndfile);
	if (err == 0) {
		free(buffer);
		lua_pushfstring(L, "Error closing %s: %s", filename, PHYSFS_getLastError());
		return ERROR;
	}
	/* skip #! if nescessary */
	entryPoint = buffer;
	if (buffer[0] == '#') {
		while (*entryPoint != '\n' && *entryPoint != EOF) {
			entryPoint++;
			fileLength--;
		}
	}
	err = luaL_loadbuffer(L, entryPoint, (size_t)fileLength, filename);
	lua_insert(L, -(narg+1));
	free(buffer);
	if (err != 0)
		return ERROR;
	lua_pushcfunction(L, error_function);
	lua_insert(L, -(narg+2));
	stackDump(L);
	n = lua_gettop(L);
	err = lua_pcall(L, narg, LUA_MULTRET, -(narg+2));
	stackDump(L);
	if (err == 0) {
		fprintf(stdout, "Finished: \"%s\"\n", filename);		
		*nres = lua_gettop(L) - (n - narg - 1);	/* calc number of results */
		return SUCCESS;
	} else
		return ERROR;
}

static int Lua_FS_runLuaFile(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	int nres = 0;
	int result = FS_runLuaFile(filename, 0, &nres);
	if ((result == ERROR) && !check_for_exit())
		return luaL_error(L, lua_tostring(L, -1));
	
	return nres;
}


static const struct luaL_Reg fileiolib[] = {
	{"addFile", Lua_FS_runLuaFile},
	{NULL, NULL}
};

int luaopen_fileio(lua_State *L, const char *parent) {
	luaL_register(L, parent, fileiolib);
	return 1;
}
