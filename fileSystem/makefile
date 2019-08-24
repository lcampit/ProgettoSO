CC = gcc
CFLAGS = -I
DEPS = bit_map.h diskDriver.h fileSystem.h
OBJ = bit_map.o diskDriver.o fileSystem.o testFileSystem.o

%.o: %.c  $(DEPS)
					$(CC) -c -o $@ $< $(CFLAGS)
testFileSystem: $(OBJ)
								$(CC) -o $@ $^ $(CFLAGS)
term:
		$(CC) bit_map.c diskDriver.c fileSystem.c SimpleFS.c -o SimpleFS $(CFLAGS)

terminal:
		./SimpleFS

test:
	./testFileSystem

all:
	make
	make term
	make testFileSystem
	make clean


.PHONY: clean

clean:
		rm -f *.o *~
