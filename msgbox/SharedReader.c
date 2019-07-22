#include "SharedFunctions.h"
#define SIZE 1024

int main(int argc, char** argv){

  char*name= argv[1];
  void* mem= SharedCreate(name,SIZE,1);
  while(1){
  int offset=SharedRead(mem);
  if(offset==-1) break;
  mem+=offset;
}
return 0;
}
