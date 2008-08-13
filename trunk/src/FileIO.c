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
#include <stdio.h>
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

void FS_Init(int argc, char *argv[], char **pFilename) {
	#ifdef __APPLE__
		char *ch = NULL;
		const char *basedir;
	#endif
	FILE *fh;
	char magic[4] = "000"; /* array for the magic bytes used to recognize a zip archive */
	char *dir = NULL;
	char *base = NULL;
	char ** arr = NULL;
	/* initialize PhysFS */
	if ( !PHYSFS_init(argv[0]) ) 
		error(L, "Error: Couldn't initialize PhysFS: %s.", PHYSFS_getLastError());
	
	/* allow symlinks */
	PHYSFS_permitSymbolicLinks(1);
	
	/*	on Mac OS X applications are packed inside a folder with the ending .app;
		try to set the search path to this folder;
		if a file is not found in the base dir (the dir containing the app bundle)
		it is searched inside the bundle */
	#ifdef __APPLE__
		ch = strstr(argv[0], "/Contents/MacOS");
		if (ch != NULL) {
			/* substite the 'C' of 'Contents/MacOS' with a string terminator */
			*(ch+1) = '\0';
			if (*pFilename == NULL)
				/* if no filename was selected */
				chdir(argv[0]);
			/* append app folder to search path */
			if ( !PHYSFS_addToSearchPath(argv[0], 1) )
				error(L,	"Error: Could not add application folder" 
							"to search path: %s.", PHYSFS_getLastError() );
			*(ch+1) = 'C';
		} else {
			basedir = PHYSFS_getBaseDir();
			if (*pFilename == NULL)
				chdir(basedir);
			if ( !PHYSFS_addToSearchPath(basedir, 1) )
				error(L, "Error: Could not add base dir to search path: %s.", PHYSFS_getLastError());
		}
	#else
		/* check whether we are on Linux or Unix */
		#ifndef WIN32
			/* on Linux or Unix: Try to append the share directory */
			PHYSFS_addToSearchPath(SHARE_DIR, 1);
		#endif
		/* on every system but OS X, append base dir to search path */
		if ( !PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1) )
			error(L, "Error: Could not add base dir to search path: %s.", PHYSFS_getLastError());
	#endif
	
	/* 
	 * if a Lua file is given, try to mount the parent directory and change 
	 * the working directory 
	 * if an archive or directory is given, try to mount it 
	 */
	if (*pFilename == NULL) {
		*pFilename = DEFAULT_FILE;
		if (PHYSFS_exists(*pFilename) == 0) {
			/* if default file not exists */
			if (PHYSFS_exists(DEFAULT_ARCHIVE) != 0) { 
				/* if default archive exists, prepend to search path */
				if ( !PHYSFS_addToSearchPath(DEFAULT_ARCHIVE, 0) )
					error(L,	"Error: Could not add default archive '"
								DEFAULT_ARCHIVE "' to search path: %s.", 
								PHYSFS_getLastError());			
			} else
				error(L,	"Error: "
							"Neither the default Lua file '" DEFAULT_FILE 
							"' nor the default archive '" DEFAULT_ARCHIVE 
							"' could be found.");
		}				
	} else {
		/* try to change the working directory (only successful if directory is given) */
		if (chdir(*pFilename) == 0) {
			/* prepend the new working directory to the search path */
			if (!PHYSFS_addToSearchPath(".", 0))
				error(L, "Error: Could not add directory '%s' to search path: %s.", argv[1], PHYSFS_getLastError());
			*pFilename = DEFAULT_FILE;
		} else {
			/* chdir was unsuccessful -> archive or Lua file was probably given on command line */
			splitPath(*pFilename, &dir, &base);
			/* change the working directory to the directory with the archive or the Lua file */
			chdir(dir);
			/* check if it's an archive; only zip is supported, so we check for the magic numbers */
			fh = fopen(base, "r");
			if (fh == NULL)
				error(L, "Error: Could not open file '%s' for reading.", argv[1]);
			fread(magic, 1, 4, fh);
			fclose(fh);
			/* look for the four signature bytes that every zip file has */
			if (magic[0] == 0x50 && magic[1] == 0x4B && magic[2] == 0x03 && magic[3] == 0x04) {
				fprintf(stdout, "Found zip archive: %s\n", base);
				if (!PHYSFS_addToSearchPath(base, 0))
					error(L, "Error: Could not add archive '%s' to search path: %s.", argv[1], PHYSFS_getLastError());
				*pFilename = DEFAULT_FILE;
			} else {
				fprintf(stdout, "Found Lua file: %s\n", base);
				/* prepend the new working directory to the search path */
				if (!PHYSFS_addToSearchPath(".", 0))
					error(L, "Error: Could not add directory containing '%s' to search path: %s.", base, PHYSFS_getLastError());
				/* change the filename to its basename -> later call to FS_runLuaFile will find it in the path */
				*pFilename = base;
			}
		}
	}
	
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
	free(buffer);
	if (err != 0)
		return ERROR;
	lua_insert(L, -(narg+1));
	lua_pushcfunction(L, error_function);
	lua_insert(L, -(narg+2));
	n = lua_gettop(L);
	err = lua_pcall(L, narg, LUA_MULTRET, -(narg+2));
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
