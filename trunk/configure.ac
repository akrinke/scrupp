AC_INIT([scrupp], [0.4])
AC_CONFIG_SRCDIR([src/Main.c])
AC_CONFIG_AUX_DIR([platforms/unix])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([dist-bzip2])

AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_INSTALL
AC_PROG_RANLIB

# Check whether we're using mingw32
case $host in
  *mingw32*) LUASOCKET_SYSTEM='wsocket.$(OBJEXT)' ;
             LUASOCKET_LIBS='-lws2_32' ;
             WINMAIN='SDL_win32_main.$(OBJEXT)' ;;
  *)         LUASOCKET_SYSTEM='usocket.$(OBJEXT)' ;;
esac
AC_SUBST([LUASOCKET_SYSTEM])
AC_SUBST([WINMAIN])

# Checks for libraries:

# Check for libm
AC_CHECK_LIB([m], [ceil])

# Check for pkg-config
PKG_PROG_PKG_CONFIG

# Check for LuaJIT / Lua
PKG_CHECK_MODULES([LUAJIT], [luajit],
  [AC_DEFINE([HAVE_LUAJIT], [1], [Use LuaJIT])],
  [AC_ARG_WITH([lua-pkg],
    AC_HELP_STRING([--with-lua-pkg=ARG],[name of Lua pkg-config script [[default=lua5.1]]]),
    [LUA_PKG_NAME=${withval%.pc}],
    [LUA_PKG_NAME=lua5.1
      for L in lua5.1 lua51 lua-5.1 lua
      do
        AC_MSG_CHECKING([for $L.pc])
        if ${PKG_CONFIG} --exists ${L}
        then
          AC_MSG_RESULT([yes])
          LUA_PKG_NAME=${L}
          break
        else
          AC_MSG_RESULT([no])
        fi
      done
    ])
  PKG_CHECK_MODULES(LUA, [${LUA_PKG_NAME} >= 5.1 ${LUA_PKG_NAME} < 5.2], ,
    [AC_CHECK_LIB(lua, luaL_newstate, ,
                  AC_MSG_ERROR([Lua development files are required to compile Scrupp])
    )
  ])
 ])

# Check for PhysFS
AC_CHECK_LIB(physfs, PHYSFS_init, ,
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
PKG_CHECK_MODULES(CAIRO,cairo, ,
                  AC_MSG_ERROR([Cairo development files are required to compile Scrupp])
)

# Check for libpng
PKG_CHECK_MODULES(LIBPNG,libpng, ,
                  AC_MSG_ERROR([libpng development files are required to compile Scrupp])
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
AC_CHECK_LIB(SDL_image, IMG_Load_RW, ,
             AC_MSG_ERROR([SDL_image development files are required to compile Scrupp])
)

# Check for SDL_mixer
AC_CHECK_LIB(SDL_mixer, Mix_OpenAudio, ,
             AC_MSG_ERROR([SDL_mixer development files are required to compile Scrupp])
)

# Check for SDL_ttf
AC_CHECK_LIB(SDL_ttf, TTF_Init, ,
             AC_MSG_ERROR([SDL_ttf development files are required to compile Scrupp])
)

# Check for SMPEG
SMPEG_VERSION=0.4.4
AM_PATH_SMPEG($SMPEG_VERSION, ,
              AC_MSG_ERROR([The development files of SMPEG >= $SMPEG_VERSION are required to compile Scrupp])
)

# Check for GTK-2.0
AC_ARG_ENABLE(gtk, [AC_HELP_STRING([--disable-gtk],
        [compile without support for GTK+ error message boxes])],
    enable_gtk="$enableval", enable_gtk="yes")

if test "x$enable_gtk" = "xyes" ; then
    PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= 2.0.0], 
    [
        AC_DEFINE([USE_GTK], [1], [Define to 1 if you have gtk2 installed.])
    ],[
        AC_MSG_RESULT(no)
        AC_MSG_ERROR([
        
You must have the GTK+ 2.0 development headers installed to compile Scrupp
with support for error message boxes. 
If you want to disable the gtk message boxes then specify --disable-gtk when 
running configure.])
    ])
fi

# Check for debug flag
AC_ARG_ENABLE(debug, [AC_HELP_STRING([--enable-debug],
        [compile with debugging on])], AC_DEFINE(DEBUG), AC_DEFINE(NDEBUG))

CFLAGS="$CFLAGS $LUAJIT_CFLAGS $LUA_CFLAGS $GL_CFLAGS $CAIRO_CFLAGS $LIBPNG_CFLAGS $SDL_CFLAGS $SMPEG_CFLAGS $GTK_CFLAGS"
LIBS="$LIBS $LUAJIT_LIBS $LUA_LIBS $GL_LIBS $CAIRO_LIBS $LIBPNG_LIBS $SDL_LIBS $SMPEG_LIBS $GTK_LIBS $LUASOCKET_LIBS"
AC_OUTPUT([Makefile
           src/Makefile
           src/chipmunk/Makefile
           src/luafilesystem/Makefile
           src/lua-oocairo/Makefile
           src/luasocket/Makefile
           src/werechipmunk/Makefile
           doc/Makefile
           doc/luafilesystem/Makefile
           doc/lua-oocairo/Makefile
           doc/luasocket/Makefile
           examples/Makefile
           examples/cairo/Makefile
           examples/cairo/images/Makefile
           examples/chipmunk/Makefile
           examples/luasocket/Makefile
           fonts/Makefile
           scripts/Makefile
           scripts/interface/Makefile
           scripts/socket/Makefile
          ])
