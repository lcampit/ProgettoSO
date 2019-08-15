CC = gcc
CFLAGS = -I.
DEPS = SharedFunctions.h
OBJ1 = SharedFunctions.o SharedWriter.o
OBJ2 = SharedFunctions.o SharedReader.o
LIBS = -lrt -pthread

%.o: %.c  $(DEPS)
					$(CC) -c -o $@ $< $(CFLAGS)

writer: $(OBJ1)
						$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

reader: $(OBJ2)
						$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

runwriter:
		./writer

runreader:
		./reader

all:
	make
	make writer
	make reader

.PHONY: clean

clean:
	rm -f *.o *~ reader writer
