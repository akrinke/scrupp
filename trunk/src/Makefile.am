bin_PROGRAMS = scrupp
scrupp_SOURCES = \
	FileIO.h FileIO.c \
	Font.h Font.c \
	Graphics.h Graphics.c \
	Keyboard.h Keyboard.c \
	Macros.h Macros.c \
	Main.h Main.c \
	Mouse.h Mouse.c \
	Movie.h Movie.c \
	Sound.h Sound.c \
	physfsrwops.h physfsrwops.c \
	IMG_savepng.h IMG_savepng.c
SUBDIRS = \
	chipmunk \
	luafilesystem \
	lua-oocairo \
	luasocket \
	werechipmunk
LDADD = \
	luafilesystem/libluafilesystem.a \
	lua-oocairo/libluaoocairo.a \
	luasocket/libluasocket.a \
	werechipmunk/libwerechipmunk.a \
	chipmunk/libchipmunk.a
AM_CFLAGS = -DSHARE_DIR=\"$(pkgdatadir)\"