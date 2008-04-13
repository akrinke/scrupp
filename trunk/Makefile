CC = gcc
CFLAGS = -c $(shell sdl-config --cflags) -Wall -ggdb
LDFLAGS = -lGL -lSDL_image -lSDL_mixer -lSDL_ttf $(shell sdl-config --libs) $(shell pkg-config --libs lua) -lphysfs

SOURCES = FileIO.c \
Font.c \
Graphics.c \
Keyboard.c \
Macros.c \
Main.c \
Mouse.c \
Sound.c \
physfsrwops.c

OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = scrupp
VPATH = src

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o scrupp
