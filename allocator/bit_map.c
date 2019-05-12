#include "bit_map.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void BitMap_init(BitMap* b, int size){
  b -> friendo = (unsigned char*) malloc(size*sizeof(unsigned char));
  b -> size = size;
  b -> bits = size*8;
  BitMap_setRange(&bm, 0, 0, bm.bits);
}

void BitMap_set(BitMap* b, int i) {
    b->friendo[i / 8] |= 1 << (i & 7);
}

void BitMap_unset(BitMap* b, int i) {
    b->friendo[i / 8] &= ~(1 << (i & 7));
}

int BitMap_get(BitMap* b, int i) {
    return b->friendo[i / 8] & (1 << (i & 7)) ? 1 : 0;
}

void BitMap_print(BitMap* b, int from, int to){
  int j;
  for(j = from; j < to; j++){
    printf("%d\t", BitMap_get(b, j));
  }
  printf("\n");
}

void BitMap_setRange(BitMap* b, int bit, int from, int to){
  int j;
  for(j = from; j < to; j++){
    if(bit) BitMap_set(b, j);
    else BitMap_unset(b, j);
  }
  return;
}

void BitMap_printInfo(BitMap* b){
  printf("----------\n");
  printf("size: %d \t", b->size);
  printf("bits: %d\n", b->bits);
  printf("----------\n");
  return;
}
