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
        printf("Before allocation:\n");
        BitMap_print(buff->check, 0, 10);
        for(s = 0; s < numbers; s++){
          BitMap_set(buff->check, i+s);
        }
        printf("After allocation:\n");
        BitMap_print(buff->check, 0, 10);
        return &(buff->memory[i]);
      }
    }
  }
  printf("not enough memory\n");
  return NULL;
}
