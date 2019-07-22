#include "SharedFunctions.h"
#include <string.h>
#define SIZE 1024

int main(int argc, char** argv){
  char* name = argv[1];
  void* mem = SharedCreate(name, SIZE, 0);
  char* text = (char*) malloc(sizeof(char) * 60);

  int i = 0;
  while(1){
    while(i < 60){
      int res = read(0, text + i, 1);
      if(res == 1){
        if (text[i] == '\n') break;
        i++;
      }
      else{
        exit(-1);
      }
    }
    if(!strcmp(text, "quit\n")){
      int no_use = SharedWrite(text, mem);
      break;
    }
    int offset = SharedWrite(text, mem);
    mem += offset;
    i = 0;
    free(text);
    text = (char*) malloc (sizeof(char) * 60);
  }
  free(text);
  shm_unlink(name);
  return 0;
}
