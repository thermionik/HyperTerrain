## NeHe's GLframe ported to SDL
## Fabio 'SnowDruid' Franchello
## snowdruid@tiscalinet.it

## Compiler
CC=g++
## Linker
LD=$(CC)
## Flags
CPPFLAGS = -Wall -g -DLINUX `sdl-config --cflags`
LFLAGS = `sdl-config --libs` -lGL -lGLU

HEADERFILES = main.h

OBJS = main.o app.o error.o

SRCS = main.cpp app.cpp error.cpp

all : GLframe
	chmod g+rw *.o

clean :
	rm -rf GLframe *.o

GLframe : ${OBJS}
	$(LD) -o $@ ${OBJS} ${LFLAGS}

depend :
	makedepend ${SRCS}

