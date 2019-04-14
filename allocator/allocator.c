#include <stdlib.h>
#include <stdio.h>
#include "allocator.h"

void* my_alloc(buffer* buff, int size){
  int i = 0;
  int j = 0;
  int numbers = size/(buff->block_size);
  if(size%(buff->block_size) != 0) numbers++;
  printf("block numbers = %d\n", numbers);
  for(; i <= (buff->num)-numbers; i++){
    if(BitMap_get(buff->check, i) == 0){
      if(BitMap_checkRange(buff->check, i, i+numbers, 0)){
        int s;
        for(s = 0; s < numbers; s++){
          BitMap_set(buff->check, i+s);
        }
        return (void*)&(buff->memory[i]);
      }
    }
  }
  printf("not enough memory\n");
  return NULL;
}

void my_free(buffer* b, void* ptr, int size){
  int ind = ptr;
  int start = &(b->memory[0]);
  int number = (ind - start)/4;
  int i;
  int num = size/b->block_size;
  for(i = 0; i < num; i++){
    BitMap_unset(b->check, number+i);
  }
}
