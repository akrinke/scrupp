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

void FS_Quit() {
	fprintf(stdout,"Running FS_Quit.\n");
	PHYSFS_deinit();
}

void FS_Init(int argc, char *argv[], int e_flag, char **filename) {
	char *dir = NULL;
	char *base = NULL;
	char ** arr = NULL;
	/* initialize PhysFS */
	if ( !PHYSFS_init(argv[0]) ) {
		fprintf(stderr,"Error: Couldn't initialize PhysFS: %s.\n", PHYSFS_getLastError());
		exit(1);
	}

	/* in every case: append base dir to search path */
	if ( !PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1) ){
		fprintf(stderr,"Error: Could not add base dir to search path: %s.\n", PHYSFS_getLastError());
		exit(1);
	}

	/* if an archive (or directory) is given, try to mount it */
	if (!e_flag && argv[1] != NULL) {
		/* try to change the working directory (only successful if directory is given) */
		if (chdir(argv[1]) == 0) {
			/* prepend the new working directory to the search path */
			if (!PHYSFS_addToSearchPath(".", 0)) {
				fprintf(stderr,"Error: Could not add directory '%s' to search path: %s.\n", argv[1], PHYSFS_getLastError());
				exit(1);
			}
		} else {
			/* chdir was unsuccessful -> archive was probably given on command line */
			splitPath(argv[1], &dir, &base);
			/* change the working directory to the directory with the archive */
			chdir(dir);
			if (!PHYSFS_addToSearchPath(base, 0)) {
				fprintf(stderr,"Error: Could not add archive '%s' to search path: %s.\n", argv[1], PHYSFS_getLastError());
				exit(1);
			}
		}
	/* if a filname to execute is given, add its directory to the search path */
	} else if (e_flag && *filename != NULL) {
		splitPath(*filename, &dir, &base);
		/* change the working directory to the directory with the Lua file */
		chdir(dir);
		/* prepend the new working directory to the search path */
		if (!PHYSFS_addToSearchPath(".", 0)) {
			fprintf(stderr,"Error: Could not add directory containing '%s' to search path: %s.\n", base, PHYSFS_getLastError());
			exit(1);
		}
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

int FS_runLuaFile(const char *filename, int narg) {
	char *buffer;		/* buffer for the file */
	char *entryPoint;	/* entry point of file (differs from buffer, if "#!" in the first line is skipped */
	int error;
	PHYSFS_file *Hndfile = NULL;
	PHYSFS_sint64 fileLength, size;
	if (PHYSFS_exists(filename) == 0) {
		fprintf(stderr, "Error executing %s: file not found.\n", filename);
		lua_pop(L, narg);
		return 0;
	}
	fprintf(stdout, "Executing \"%s\"...\n", filename);
	Hndfile = PHYSFS_openRead(filename); /* open file to read! */
	if (Hndfile == NULL) {
		fprintf(stderr, "Error while reading from %s: %s\n", filename, PHYSFS_getLastError());
		lua_pop(L, narg);
		return 0;
	}
	size = PHYSFS_fileLength(Hndfile);
	if (size == -1) {
		fprintf(stderr, "Error while determining the size of %s\n", filename);
		lua_pop(L, narg);
		return 0;
	}
	buffer = (char *)malloc((unsigned int)size);
	if (buffer == NULL) { 
		fprintf(stderr, "Error loading %s: Insufficient memory available\n", filename);
		lua_pop(L, narg);
		return 0;
	}
	fileLength = PHYSFS_read(Hndfile, buffer, 1, (unsigned int)size);
	if (fileLength < size) {
		fprintf(stderr, "Error while reading from %s: %s\n", filename, PHYSFS_getLastError());
		lua_pop(L, narg);
		return 0;
	}
	/* close the file */
	error = PHYSFS_close(Hndfile);
	if (error == 0) {
		fprintf(stderr, "Error closing %s: %s\n", filename, PHYSFS_getLastError());
		lua_pop(L, narg);
		return 0;
	}
	/* skip #! if nescessary */
	entryPoint = buffer;
	if (buffer[0] == '#') {
		while (*entryPoint != '\n' && *entryPoint != EOF) {
			entryPoint++;
			fileLength--;
		}
	}
	error =	luaL_loadbuffer(L, entryPoint, (size_t)fileLength, filename);
	lua_insert(L, -(narg+1));
	if (!error)
		error = lua_pcall(L, narg, 0, 0);
	else
		lua_pop(L, narg);
	//if (error)
	//	fprintf(stderr, "Error executing %s: [%s\n", filename, lua_tostring(L, -1)+8);
	fprintf(stdout, "Finished: \"%s\"\n", filename);
	free(buffer);
	return error;
}

int Lua_FS_runLuaFile(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	int error = FS_runLuaFile(filename, 0);
	if (error && !check_for_exit())
		fprintf(stderr, "Error executing %s: [%s\n", filename, lua_tostring(L, -1)+8);
	return 0;
}


static const struct luaL_Reg fileiolib[] = {
	{"addFile", Lua_FS_runLuaFile},
	{NULL, NULL}
};

int luaopen_fileio(lua_State *L, const char *parent) {
	luaL_register(L, parent, fileiolib);
	return 1;
}
