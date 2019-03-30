#include <stdlib.h>
#include <stdint.h>
#include "bit_map.h"

typedef struct {
  int* memory;
  int block_size;
  int num;
  BitMap*check;
}buffer;

void* my_alloc(buffer* buff, int size);
