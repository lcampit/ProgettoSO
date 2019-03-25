CC=gcc
CFLAGS=-I.
bitmap: testmap.o bit_map.o
	$(CC) -o bitmapTest testmap.o bit_map.o


.PHONY: clean

clean:
	rm -f *.o $(objects)
