/*
** $Id$
** File I/O: Access to the virtual file system
** See Copyright Notice in COPYRIGHT
*/

#include "Main.h"
#include "Macros.h"
#include "FileIO.h"

#include <luaconf.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <physfs.h>

/* copied from lua.c of the Lua distribution */
static const char *pushnexttemplate (lua_State *L, const char *path) {
	const char *l;
	while (*path == *LUA_PATHSEP) path++;  /* skip separators */
	if (*path == '\0') 
		return NULL;  /* no more templates */
	l = strchr(path, *LUA_PATHSEP);  /* find next separator */
	if (l == NULL) 
		l = path + strlen(path);
	lua_pushlstring(L, path, l - path);  /* template */
	return l;
}

/* based on code from lua.c of the Lua distribution */
static const char *findfile (lua_State *L, const char *name) {
	const char *path;
	name = luaL_gsub(L, name, ".", "/");
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "scrupppath");
	path = lua_tostring(L, -1);
	if (path == NULL)
		luaL_error(L, LUA_QL("package.scrupppath") " must be a string");
	lua_pushstring(L, "");  /* error accumulator */
	while ((path = pushnexttemplate(L, path)) != NULL) {
		const char *filename;
		filename = luaL_gsub(L, lua_tostring(L, -1), LUA_PATH_MARK, name);
		if (PHYSFS_exists(filename))  /* does file exist? */
			return filename;  /* return that file name */
		lua_pop(L, 2);  /* remove path template and file name */ 
		lua_pushfstring(L, "\n\tno file " LUA_QL("searchpath://%s") , filename);
		lua_concat(L, 2);
	}
	return NULL;  /* not found */
}


/* A loader which looks for modules in the physfs virtual filesystem */
/* based on code from lua.c of the Lua distribution */
static int loader_PhysFS(lua_State *L) {
	const char *filename;
	const char *name = luaL_checkstring(L, 1);
	int result;
	filename = findfile(L, name);
	if (filename == NULL) {
		return 1; /* library not found in this path */
	}
	result = FS_loadFile(L, filename);
	if (result == FILEIO_ERROR) {
		return luaL_error(L, 	"error loading module " LUA_QS " from file " 
								LUA_QL("searchpath://%s") ":\n\t%s",
								lua_tostring(L, 1), filename, lua_tostring(L, -1));
	}
	return 1; /* library loaded successfully */
}
	
static void FS_Quit(void) {
	fprintf(stdout,"Running FS_Quit.\n");
	PHYSFS_deinit();
}

/* extracts the dirname and the basename from the path */
static void splitPath(const char *path, char **dirname, char **basename) {
	char *ptr;
	const char *dirsep = NULL;
	*basename = (char *)path;
	*dirname = ".";
	dirsep = PHYSFS_getDirSeparator();
	if (strlen(dirsep) == 1) { /* fast path. */
		ptr = strrchr(path, *dirsep);
	}
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

void FS_Init(int argc, char *argv[], char **pFilename) {
	#ifdef __MACOSX__
		char *ch = NULL;
		const char *basedir;
	#endif
	FILE *fh;
	char magic[4] = "000"; /* array for the magic bytes used to recognize a zip archive */
	char *dir = NULL;
	char *base = NULL;
	char **search_path = NULL;
	char **copy;
	
	/* initialize PhysFS */
	if (PHYSFS_init(argv[0]) ==0) {
		error(L, "Error: Could not initialize PhysFS: %s.", PHYSFS_getLastError());
	}
	
	/* allow symlinks */
	PHYSFS_permitSymbolicLinks(1);
	
	/*	on Mac OS X applications are packed inside a folder with the ending .app;
		try to set the search path to this folder;
		if a file is not found in the base dir (the dir containing the app bundle)
		it is searched inside the bundle */
	#ifdef __MACOSX__
		ch = strstr(argv[0], "/Contents/MacOS");
		if (ch != NULL) {
			/* substite the 'C' of 'Contents/MacOS' with a string terminator */
			*(ch+1) = '\0';
			if (*pFilename == NULL) {
				/* if no filename was selected */
				chdir(argv[0]);
			}
			/* append app folder to search path */
			if (PHYSFS_addToSearchPath(argv[0], 1) == 0) {
				error(L,	"Error: Could not add application folder" 
							"to search path: %s.", PHYSFS_getLastError());
			}
			*(ch+1) = 'C';
		} else {
			basedir = PHYSFS_getBaseDir();
			if (*pFilename == NULL) {
				chdir(basedir);
			}
			if (PHYSFS_addToSearchPath(basedir, 1) == 0) {
				error(L, "Error: Could not add base dir to search path: %s.", PHYSFS_getLastError());
			}
		}
	#else
		/* check whether we are on Linux or Unix */
		#ifndef __WIN32__
			/* on Linux or Unix: Try to append the share directory */
			PHYSFS_addToSearchPath(SHARE_DIR, 1);
		#endif
		/* on every system but OS X, prepend base dir to search path */
		if (PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 0) == 0) {
			error(L, "Error: Could not add base dir to search path: %s.", PHYSFS_getLastError());
		}
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
				if (PHYSFS_addToSearchPath(DEFAULT_ARCHIVE, 0) == 0) {
					error(L,	"Error: Could not add default archive '"
								DEFAULT_ARCHIVE "' to search path: %s", 
								PHYSFS_getLastError());
				}
			} else {
				error(L,	"Error: "
							"Neither the default Lua file '" DEFAULT_FILE 
							"' nor the default archive '" DEFAULT_ARCHIVE 
							"' could be found.");
			}
		}				
	} else {
		/* try to change the working directory (only successful if directory is given) */
		if (chdir(*pFilename) == 0) {
			/* prepend the new working directory to the search path */
			if (PHYSFS_addToSearchPath(".", 0) == 0) {
				error(L, "Error: Could not add directory '%s' to search path: %s", argv[1], PHYSFS_getLastError());
			}
			*pFilename = DEFAULT_FILE;
		} else {
			/* chdir was unsuccessful -> archive or Lua file was probably given on command line */
			splitPath(*pFilename, &dir, &base);
			/* change the working directory to the directory with the archive or the Lua file */
			chdir(dir);
			/* check if it's an archive; only zip is supported, so we check for the magic numbers */
			fh = fopen(base, "r");
			if (fh == NULL) {
				error(L, "Error: Could not open file '%s' for reading.", argv[1]);
			}
			fread(magic, 1, 4, fh);
			fclose(fh);
			/* look for the four signature bytes that every zip file has */
			if (magic[0] == 0x50 && magic[1] == 0x4B && magic[2] == 0x03 && magic[3] == 0x04) {
				fprintf(stdout, "Found zip archive: %s\n", base);
				if (PHYSFS_addToSearchPath(base, 0) == 0) {
					error(L, "Error: Could not add archive '%s' to search path: %s", argv[1], PHYSFS_getLastError());
				}
				*pFilename = DEFAULT_FILE;
			} else {
				fprintf(stdout, "Found Lua file: %s\n", base);
				/* prepend the new working directory to the search path */
				if (PHYSFS_addToSearchPath(".", 0) == 0) {
					error(L, 	"Error: Could not add directory containing '%s' to search path: %s", 
								base, PHYSFS_getLastError());
				}
				/* change the filename to its basename -> later call to FS_runLuaFile will find it in the path */
				*pFilename = base;
			}
		}
	}
	
	atexit(FS_Quit);

	search_path = PHYSFS_getSearchPath();
	copy = search_path;
	while (*copy != NULL) {
		printf("search path: %s\n", *copy++);
	}
	PHYSFS_freeList(search_path);
}

int FS_loadFile(lua_State *L, const char *filename) {
	char *buffer;		/* buffer for the file */
	char *entryPoint;	/* entry point of file (differs from buffer, if "#!" in the first line is skipped */
	int err;
	PHYSFS_file *Hndfile = NULL;
	PHYSFS_sint64 fileLength, size;
	
	if (PHYSFS_exists(filename) == 0) {
		lua_pushfstring(L, "Error loading '%s': file not found.", filename);
		return FILEIO_ERROR;
	}

	Hndfile = PHYSFS_openRead(filename); /* open file to read! */
	if (Hndfile == NULL) {
		lua_pushfstring(L, "Error while reading from '%s': %s", filename, PHYSFS_getLastError());
		return FILEIO_ERROR;
	}

	size = PHYSFS_fileLength(Hndfile);
	if (size == -1) {
		lua_pushfstring(L, "Error while determining the size of %s.", filename);
		return FILEIO_ERROR;
	}

	buffer = (char *)malloc((unsigned int)size);
	if (buffer == NULL) {
		lua_pushfstring(L, "Error loading %s: Insufficient memory available.", filename);
		return FILEIO_ERROR;
	}

	fileLength = PHYSFS_read(Hndfile, buffer, 1, (unsigned int)size);
	if (fileLength < size) {
		free(buffer);
		lua_pushfstring(L, "Error while reading from %s: %s", filename, PHYSFS_getLastError());
		return FILEIO_ERROR;
	}
	/* close the file */
	err = PHYSFS_close(Hndfile);
	if (err == 0) {
		free(buffer);
		lua_pushfstring(L, "Error closing %s: %s", filename, PHYSFS_getLastError());
		return FILEIO_ERROR;
	}
	/* skip #! if nescessary */
	entryPoint = buffer;
	if (buffer[0] == '#') {
		while ((*entryPoint != 0x0D) && (*entryPoint != 0x0A) && (*entryPoint != EOF)) {
			entryPoint++;
			fileLength--;
		}
	}
	err = luaL_loadbuffer(L, entryPoint, (size_t)fileLength, filename);
	free(buffer);	
	if (err != 0) {
		/* error message is on the stack */
		return FILEIO_ERROR;
	}
	
	return FILEIO_SUCCESS;
}

static int Lua_FS_dofile(lua_State *L) {
	const char *filename = luaL_optstring(L, 1, NULL);
	int n = lua_gettop(L);
	if (PHYSFS_exists(filename)) {
		if (FS_loadFile(L, filename) == FILEIO_ERROR) {
			return lua_error(L);
		}
	} else if (luaL_loadfile(L, filename) != 0) {
		return lua_error(L);
	}
	lua_call(L, 0, LUA_MULTRET);
	return lua_gettop(L) - n;
}

static int Lua_FS_fileExists(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	lua_pushboolean(L, PHYSFS_exists(filename));
	
	return 1;
}

static int Lua_FS_isDirectory(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	lua_pushboolean(L, PHYSFS_isDirectory(filename));
	
	return 1;
}

static int Lua_FS_isSymbolicLink(lua_State *L) {
	const char *filename = luaL_checkstring(L, 1);
	lua_pushboolean(L, PHYSFS_isSymbolicLink(filename));
	
	return 1;
}

static int Lua_FS_getSearchPath(lua_State *L) {
	char **search_path = PHYSFS_getSearchPath();
	char **copy = search_path;
	int n = 1;
	lua_newtable(L);
	while (*copy != NULL) {
		lua_pushstring(L, *copy++);
		lua_rawseti(L, -2, n++);
	}
	PHYSFS_freeList(search_path);
	
	return 1;
}

static int Lua_FS_setSearchPath(lua_State *L) {
	const char *str;
	char **search_path = PHYSFS_getSearchPath();
	char **copy = search_path;
	int n, maxn;

	luaL_checktype(L, 1, LUA_TTABLE);

	/* clear the search path */
	while (*copy != NULL) {
		PHYSFS_removeFromSearchPath(*copy++);
	}
	
	PHYSFS_freeList(search_path);
	maxn = lua_objlen(L, 1);
	for (n = 1; n <= maxn; n++) {
		lua_pushinteger(L, n);
		lua_gettable(L, -2);
		str = lua_tostring(L, -1);
		if (PHYSFS_addToSearchPath(str, 1) == 0) {
			return luaL_error(L,	"Error: Could not add directory or archive '%s' "
									"to search path: %s", str, PHYSFS_getLastError());
		}
		/* remove the string */
		lua_pop(L, 1);
	}

	return 0;
}

/* auxiliary mark (for internal use) */
#define AUXMARK         "\1"

/* copied from lua.c of the Lua distribution */
static void setpath (lua_State *L, const char *fieldname, const char *envname,
                                   const char *def) {
	const char *path = getenv(envname);
	if (path == NULL)  /* no environment variable? */
		lua_pushstring(L, def);  /* use default */
	else {
		/* replace ";;" by ";AUXMARK;" and then AUXMARK by default path */
		path = luaL_gsub(L, path, 	LUA_PATHSEP LUA_PATHSEP,
									LUA_PATHSEP AUXMARK LUA_PATHSEP);
		luaL_gsub(L, path, AUXMARK, def);
		lua_remove(L, -2);
	}
	lua_setfield(L, -2, fieldname);
}

static const struct luaL_Reg fileiolib[] = {
	{"fileExists", Lua_FS_fileExists},
	{"isDirectory", Lua_FS_isDirectory},
	{"isSymbolicLink", Lua_FS_isSymbolicLink},
	{"getSearchPath", Lua_FS_getSearchPath},
	{"setSearchPath", Lua_FS_setSearchPath},
	{NULL, NULL}
};

int luaopen_fileio(lua_State *L, const char *parent) {
	luaL_register(L, parent, fileiolib);
	
	lua_pushcfunction(L, Lua_FS_dofile);
	lua_setglobal(L, "dofile");
	
	/* insert the custom PhysFS loader into the loaders table */
	lua_getglobal(L, "table");
	lua_getfield(L, -1, "insert");
	lua_remove(L, -2);
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaders");
	lua_remove(L, -2);
	lua_pushinteger(L, 2);
	lua_pushcfunction(L, loader_PhysFS);
	lua_call(L, 3, 0);
	
	/* set field `scrupppath' */
	lua_getglobal(L, "package");	
	setpath(L, "scrupppath", SCRUPP_PATH, SCRUPP_PATH_DEFAULT);
	lua_remove(L, -1);
	
	return 1;
}
