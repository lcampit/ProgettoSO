#include <stdlib.h>
#include <stdio.h>

#define dim 4
#define max 12800

typedef struct {
  int* memory;
  int size;
  int num;
  int*check;
} buffer;



void* my_alloc(buffer* buff, int size){
  int i = 0;
  int j = 0;
  int numbers = size/dim;
  if(size%dim != 0) numbers++;
  while(i++<(buff->num)-numbers){
    if(buff->check[i] == 0){
      int s = i;
      for(; s++; s < numbers){
        buff->check[s] == 1;
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
  buf.memory = mem;
  buf.num = max/dim;
  int check1[max/dim];
  int j;
    for(j = 0; j++; j < buf.num) check1[j] = 0;
  buf.check = check1;
  int* prova1 = (int*)my_alloc(&buf, 32);
  *prova1 = 5;
  printf("%d\n", *prova1);

}
