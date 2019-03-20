#include <stdio.h>
#include <stdlib.h>
#include "bit_map.h"

int main(){
  BitMap bitemappo;
  int xyz= 123456;
  uint8_t* stuffer=(uint8_t*) malloc(sizeof(uint8_t)*xyz);
 Bit_init(&bitemappo,xyz,stuffer);
 printf("%d\n",bitemappo.size );
 Bit_setBit( &bitemappo, 53, 1);
 int i=Bit_status(&bitemappo,53);
 printf("%d\n",i );
}
