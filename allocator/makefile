CC = gcc
CFLAGS = -I.
DEPS = allocator.h bit_map.h
OBJ = allocator.o bit_map.o testalloc.o
LIBS = -lm

%.o: %.c  $(DEPS)
					$(CC) -c -o $@ $< $(CFLAGS)

alloc: $(OBJ)
						$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
runalloc:
		./alloc

all:
	make
	make alloc

.PHONY: clean

clean:
	rm -f *.o *~ alloc
