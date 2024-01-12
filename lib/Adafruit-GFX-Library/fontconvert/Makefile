all: fontconvert

CC     = gcc
CFLAGS = -Wall -I c:/msys32/mingw/include/freetype2
LIBS   = -lfreetype

fontconvert: fontconvert.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@
	strip $@

clean:
	rm -f fontconvert
