#include <stdint.h>
//#include "bit_map.c"

typedef struct{
  int size;// grandezza buffer
  int bits;// numero tot di bit
  uint8_t *friendo; // buffer per il buddy
}BitMap;

int Bit_GetBytesFromBit(int n_bits);// converte bit in byte.

void Bit_init(BitMap* bm,int n_size,uint8_t* buffo); //inizializzazione

int Bit_status( BitMap*bm,int index); //funzione fondamentale di controllo

void Bit_setBit(BitMap*bm,int index, int situa);//setta un bit a 0 o a 1;
