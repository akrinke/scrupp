# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# See OPTIONS for possible values.
OPTION= none

CC= gcc
CFLAGS= -c $(shell sdl-config --cflags) -Wall $(MYCFLAGS)
LIBS= -lGL -lSDL_image -lSDL_mixer -lSDL_ttf \
	$(shell sdl-config --libs) $(shell pkg-config --libs lua) \
	-lphysfs -lsmpeg $(MYLIBS)

MYCFLAGS=
MYLDFLAGS=
MYLIBS=

# == END OF USER SETTINGS. NO NEED TO CHANGE ANYTHING BELOW THIS LINE =========

OPTIONS= with-gtk without-gtk

SOURCES = FileIO.c \
Font.c \
Graphics.c \
Keyboard.c \
Macros.c \
Main.c \
Mouse.c \
Sound.c \
Movie.c \
physfsrwops.c

OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = scrupp
VPATH = src

default: $(OPTION)

all: $(SOURCES) $(EXECUTABLE)

none:
	@echo "Please choose an option:"
	@echo "   $(OPTIONS)"
	
without-gtk:
	$(MAKE) all
	
with-gtk:
	$(MAKE) all MYCFLAGS="$(shell pkg-config --cflags gtk+-2.0) -DUSE_GTK" \
	MYLIBS=$(shell pkg-config --libs gtk+-2.0) 

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LIBS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o scrupp
