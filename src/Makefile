
CC = gcc
CFLAGS = -Wall -O2 -pedantic
LDFLAGS = -L/usr/X11R6/lib -lX11 -lGL -lGLU -lpng -lm -g

EXE = xfps
OBJECTS = bullet.o debug.o enemy.o error.o font.o hash.o lex.o load.o \
	main.o map.o object.o player.o status.o tex.o trig.o

.SUFFIXES: .o .c .h

all: $(EXE)

$(EXE): $(OBJECTS)
	$(CC) -o $(EXE) $(OBJECTS) $(LDFLAGS)

.c.o: $*.c
	$(CC) -c $(CFLAGS) $*.c

clean:
	rm -f *.o $(EXE) core

