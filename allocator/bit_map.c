#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct{
  int size;// grandezza buffer
  int bits;// numero tot di bit
  unsigned char *friendo; // buffer per il buddy
} BitMap;

void BitMap_init(BitMap* b, int size){
  b -> friendo = (unsigned char*) malloc(size*sizeof(unsigned char));
  b -> size = size;
  b -> bits = size*8;
  BitMap_setRange(b, 0, 0, b->bits);
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

int BitMap_checkRange(BitMap* b, int start, int end, int value){
  int j = start;
  for(; j < end; j++){
    if(BitMap_get(b, j) != value) return 0;
  }
  return 1;
}