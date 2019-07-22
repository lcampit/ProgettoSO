#include "fileSystem.h"
#define MAX_INPUT 30

int main(int argc, char** argv){
  printf("Initializing filesystem\n");
  //Creating stuff
  SimpleFS* fs = (SimpleFS*) malloc(sizeof(SimpleFS));
  DiskDriver* disk = (DiskDriver*) malloc(sizeof(DiskDriver));
  DiskDriver_init(disk, 512);
  DirectoryHandle* dh = SimpleFS_init(fs, disk);
  if(dh == NULL){
    printf("Error while creating fileSystem\n");
    return 1;
  }
  //end creating stuff
  char command[MAX_INPUT];
  char argument[MAX_INPUT];
  while(1){     //starting terminal-like behaviour
    printf("SimpleFS>> ");
    scanf("%s", command);
    if(strncmp(command, "quit", 4) == 0) break;
    printf("%s", command);
    scanf("%s", argument);
    
    printf("%s, %s\n", command, argument);
  }
  return 0;
}
