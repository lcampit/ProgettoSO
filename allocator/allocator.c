#include <stdlib.h>
#include <stdio.h>

#define dim 4
#define max 12800

typedef struct {
  int* memory;
  int block_size;
  int num;
  int*check;
} buffer;



void* my_alloc(buffer* buff, int size){
  int i = 0;
  int j = 0;
  int numbers = size/(buff->block_size);
  if(size%dim != 0) numbers++;
  for(; i <= (buff->num)-numbers; i++){
    if(buff->check[i] == 0){
      int s;
      for(s = 0; s < numbers; s++){
        buff->check[i+s] = 1;
      }
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
  int check1[max/dim];
  int j;
    for(j = 0; j < buf.num; j++) check1[j] = 0;
  buf.check = check1;
  int l;
  buffer* buf_prova = (buffer*)my_alloc(&buf, sizeof(buffer));
  *buf_prova = buf;
  printf("%d\n", buf.num);
  printf("%d\n", buf_prova->num);
}
