all: fontconvert

CC     = gcc
CFLAGS = -Wall -Ic:/msys32/mingw32/include/freetype2 -Ic:/msys32/mingw32/include
LIBS   = -libfreetype

fontconvert: fontconvert.c
	$(CC) $(CFLAGS) $< $(LIBS) -o $@

clean:
	rm -f fontconvert
