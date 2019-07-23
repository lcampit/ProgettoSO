#pragma once
#include <stdlib.h>
#include <stdint.h>
#include "bit_map.h"

typedef struct {
  unsigned char* memory;
  int block_size;
  int levels;
  BitMap*check;
  int max;
}buffer;

void* my_alloc(buffer* buff, int size);//AF,MG grants memory spaces
void my_free(buffer* b, void* ptr,int size);//AF,MG  frees memory
int buddylevel(buffer* buff,int size); //AF,MG returns level of depth of minimum memory segment
void set_children(buffer*buf,int i,int level); //AF,MG sets children in bitmap
void unset_children(buffer*buf,int i,int level); //AF,MG unsets children in bitmap
void unset_parents(buffer*buf,int i,int level); //AF,MG unsets parents in bitmap until buddies aren't free anymore
