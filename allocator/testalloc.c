#include "allocator.h"
#include "bit_map.h"
#define BLOCK_SIZE 1
#define TOTAL_SIZE 16

int main() {
  buffer* my_buff=  buffer_create(TOTAL_SIZE,BLOCK_SIZE);
   printf("bit map status before my_alloc (0 is free, 1 is used)\n");
  BitMap_print(my_buff->check,1,32);
  int ex=4;
  void* ex_array=my_alloc(my_buff,ex);
  printf("bit map status after my_alloc (0 is free, 1 is used)\n");
 BitMap_print(my_buff->check,1,32);
 my_free(my_buff,ex_array,ex);
 printf("bit map status after my_free (0 is free, 1 is used)\n" );
 BitMap_print(my_buff->check,1,32);
 buffer_destroy(my_buff);
  return 0;
}
