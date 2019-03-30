#include <stdlib.h>
#include <stdio.h>
#include "bit_map.c"
#include "allocator.h"


#define dim 24
#define max 12800

/*typedef struct {
  int* memory;
  int block_size;
  int num;
  BitMap*check;
} buffer;*/



void* my_alloc(buffer* buff, int size){
  int i = 8;
  int j = 0;
  int numbers = size/(buff->block_size);
  if(size%dim != 0) numbers++;
  printf("%d\n", numbers);
  for(; i <= (buff->num)-numbers; i++){
    if(Bit_status(buff->check, i) == 0){
      int s;
      printf("i = %d\n", i );
      printf("1:%d\n", Bit_status(buff->check, i+s+1));
      printf("2:%d\n", Bit_status(buff->check, i+s+2));
      printf("3:%d\n", Bit_status(buff->check, i+s+3));
      printf("4:%d\n", Bit_status(buff->check, i+s+4));
      printf("5:%d\n", Bit_status(buff->check, i+s+5));
      printf("6:%d\n", Bit_status(buff->check, i+s+6));
      printf("7:%d\n", Bit_status(buff->check, i+s+7));
      printf("8:%d\n", Bit_status(buff->check, i+s+8));
      for(s = 0; s < numbers; s++){
        Bit_setBit(buff->check, i+s, 1);
        printf("appena fatto sono:%d\n", Bit_status(buff->check, i+s));
      }
      printf("1:%d\n", Bit_status(buff->check, i+s+1));
      printf("2:%d\n", Bit_status(buff->check, i+s+2));
      printf("3:%d\n", Bit_status(buff->check, i+s+3));
      printf("4:%d\n", Bit_status(buff->check, i+s+4));
      printf("5:%d\n", Bit_status(buff->check, i+s+5));
      printf("6:%d\n", Bit_status(buff->check, i+s+6));
      printf("7:%d\n", Bit_status(buff->check, i+s+7));
      printf("8:%d\n", Bit_status(buff->check, i+s+8));
      return &(buff->memory[i]);
    }
  }
  printf("not enough memory\n");
  return NULL;
}

int main(){
  int mem[max];
  buffer buf ;
  buf.block_size = dim;
  buf.memory = mem;
  buf.num = max/dim;
  uint8_t* check1 = (uint8_t*)malloc(sizeof(uint8_t)*(max/dim));
  BitMap* bm = (BitMap*)malloc(sizeof(BitMap));
  Bit_init(bm, buf.num, check1);
  int j;
  int i;
  for(j = 0; j < buf.num; j++){
    Bit_setBit(bm, j, 0);
  }
  buf.check = bm;
  buffer* buf_prova = (buffer*)my_alloc(&buf, sizeof(buffer));
  *buf_prova = buf;
  printf("%d\n", buf.num);
  printf("%d\n", buf_prova->num);
}
