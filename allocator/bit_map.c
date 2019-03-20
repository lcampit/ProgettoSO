#include "bit_map.h"
#include <assert.h>
#include <stdio.h>

int Bit_GetBytesFromBit(int n_bits){
  int byte= n_bits/8; //converto bit in byte
  int resto=0;
  if(n_bits%8!=0) resto=1; //ne aggiungo 1 se ho avanzi
  return byte+resto;
};

void Bit_init(BitMap* bm,int n_bits,uint8_t* buffo){
   bm->bits=n_bits;
   bm->friendo=buffo;
   bm->size=Bit_GetBytesFromBit(n_bits);
   // alloco tutto,ovviamente
}

void Bit_setBit(BitMap* bm, int index, int situa){
  //converto bye in bit
  int byte_num=index>>3;
  assert(byte_num<bm->size);
  int bit_in_byte=byte_num&0x03;
 // se situa=1 e bit=1 non faccio nulla, altrimenti lo setto; stessa cosa con situa=0
  if (situa) {
    bm->friendo[byte_num] |= (1<<bit_in_byte);
  } else {
    bm->friendo[byte_num] &= ~(1<<bit_in_byte);
  }
}

int Bit_status(BitMap*bm,int index){
  // byte to bit
  int byte_num=index>>3;
  assert(byte_num<bm->size);
  int bit_in_byte=byte_num&0x03;
  // controllo il contenuto
  return (bm->friendo[byte_num] &(1<<bit_in_byte))!=0;
}
