#pragma once
#include <stdint.h>

typedef struct{
  int size;// grandezza buffer
  int bits;// numero tot di bit
  unsigned char *friendo; // buffer per il buddy
} BitMap;

void BitMap_init(BitMap* b, int size); //LC, MG allocates bitmap

void BitMap_set(BitMap* b, int i); //LC, MG Sets bit i to 1

void BitMap_unset(BitMap* b, int i); //LC, MG Sets bit i to 0

int BitMap_get(BitMap* b, int i); //LC, MG Fetches i-th bit

void BitMap_print(BitMap* b, int from, int to);    //LC, MG: prints bits in bitmap in indexes from - to

void BitMap_setRange(BitMap* b, int bit, int from, int end); //LC, MG: sets bits in bitmap to bit in indexes from - to

void BitMap_printInfo(BitMap* b); //LC, MG: prints info on bitmap (NOT array contents)
