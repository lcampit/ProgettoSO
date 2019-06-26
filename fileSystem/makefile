CC = gcc
CFLAGS = -I
DEPS = bit_map.h diskDriver.h fileSystem.h
OBJ = bit_map.o diskDriver.o fileSystem.o testFileSystem.o

%.o: %.c  $(DEPS)
					$(CC) -c -o $@ $< $(CFLAGS)
testFileSystem: $(OBJ)
								$(CC) -o $@ $^ $(CFLAGS)

test:
	./testFileSystem
.PHONY: clean

clean:
		rm -f *.o *~
