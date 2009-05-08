AC_INIT([scrupp], [0.2])
AC_CONFIG_SRCDIR([src/Main.c])
AC_CONFIG_AUX_DIR([platforms/unix])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([dist-bzip2])
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for libraries:

# Check for Lua
AX_LUA_HEADERS
AX_LUA_LIBS
AX_LUA_LIB_VERSION(501)

# Check for PhysFS
AC_CHECK_LIB(	physfs, PHYSFS_init, ,
				AC_MSG_ERROR([PhysicsFS (physfs) development files are required to compile Scrupp])
)
AC_CHECK_HEADER(physfs.h, ,
				AC_MSG_ERROR([PhysicsFS (physfs) development files are required to compile Scrupp])
)

# Check for OpenGL
AX_CHECK_GL
if test "x$no_gl" = "xyes"; then
	AC_MSG_ERROR([OpenGL is required to compile Scrupp])
fi

# Check for Cairo
PKG_CHECK_MODULES(	CAIRO,cairo, ,
					AC_MSG_ERROR([Cairo development files are required to compile Scrupp])
)

# Check for SDL
SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION, ,
			AC_MSG_ERROR([SDL >= $SDL_VERSION is required to compile Scrupp])
)
AC_CHECK_HEADER(SDL/SDL_opengl.h, ,
				AC_MSG_ERROR([SDL_opengl.h is required to compile Scrupp])
)

# Check for SDL_image
AC_CHECK_LIB(	SDL_image, IMG_Load_RW, ,
				AC_MSG_ERROR([SDL_image development files are required to compile Scrupp])
)

# Check for SDL_mixer
AC_CHECK_LIB(	SDL_mixer, Mix_OpenAudio, ,
				AC_MSG_ERROR([SDL_mixer development files are required to compile Scrupp])
)

# Check for SDL_ttf
AC_CHECK_LIB(	SDL_ttf, TTF_Init, ,
				AC_MSG_ERROR([SDL_ttf development files are required to compile Scrupp])
)

# Check for SMPEG
SMPEG_VERSION=0.4.4
AM_PATH_SMPEG(	$SMPEG_VERSION, ,
				AC_MSG_ERROR([The development files of SMPEG >= $SMPEG_VERSION are required to compile Scrupp])
)

# Check for GTK-2.0
AC_ARG_ENABLE(gtk, [AC_HELP_STRING([--disable-gtk],
		[compile without support for GTK+ error message boxes])],
	enable_gtk="$enableval", enable_gtk="yes")

if test "x$enable_gtk" = "xyes" ; then
	PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= 2.0.0], 
	[
		AC_DEFINE([USE_GTK], [1], [Define to 1 if you have gtk2 installes.])
	],[
		AC_MSG_RESULT(no)
		AC_MSG_ERROR([
		
You must have the GTK+ 2.0 development headers installed to compile Scrupp
with support for error message boxes. 
If you want to disable the gtk message boxes then specify --disable-gtk when 
running configure.])
	])
fi

#AC_DEFINE([SHARE_DIR], "$(pkgdatadir)")

CFLAGS="$CFLAGS $LUA_INCLUDE $GL_CFLAGS $CAIRO_CFLAGS $SDL_CFLAGS $SMPEG_CFLAGS $GTK_CFLAGS"
LIBS="$LIBS $LUA_LIB $GL_LIBS $CAIRO_LIBS $SDL_LIBS $SMPEG_LIBS $GTK_LIBS"
AC_OUTPUT([Makefile
		   src/Makefile
		   src/lua-oocairo/Makefile
		   src/luasocket/Makefile
		   doc/Makefile
		   doc/lua-oocairo/Makefile
		   doc/luasocket/Makefile
		   examples/Makefile
		   examples/cairo/Makefile
		   examples/cairo/images/Makefile
		   examples/luasocket/Makefile
		   fonts/Makefile
		   scripts/Makefile
		   scripts/socket/Makefile
		  ])
