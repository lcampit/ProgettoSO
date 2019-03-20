#include <stdlib.h>
#include <stdio.h>

#define dim 128
#define max 12800

typedef struct {
  int* memory;
  int size;
  int num;
  int*check;
} buffer;



int* my_alloc(buffer* buff, int size){
  int i;
  for(i = 0; i++; i<buff->num){
    if(buff->check[i] == 0){
      buff->check[i] == 1;
      break;
    }
  }
  return &(buff->memory[i]);
}

int main(){
  int mem[max];
  buffer* buf = (buffer*)malloc(sizeof(buffer));
  buf->memory = mem;
  printf("ciao\n");
  buf->num = max/dim;
  printf("ciao\n");
  int check1[max/dim];
  printf("ciao\n");
  int j;
    for(j = 0; j++; j < buf->num) check1[j] = 0;
  buf->check = check1;
  int* prova1 = my_alloc(buf, 32);
  *prova1 = 5;
  printf("%d\n", *prova1);

}
