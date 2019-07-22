#include "fileSystem.h"
#define MAX_INPUT 300
#define MAXBUFFER 150

void print_help(){
  printf("Here's a list of possible commands to use in this shell:\n");
  printf("\ttouch filename, creates a file in current directory\n");
  printf("\tmkdir dirname, creates a dir in current directory\n");
  printf("\tls, prints contents of current directory\n");
  printf("\tcd dir, switches to dir in current directory\n");
  printf("\trmfile filename, deletes file in current directory\n");
  printf("\trmdir dirname, deletes dir in current directory\n");
  printf("\tquit, releases resources and quits program\n");
  printf("\thelp, prints this help\n");
}

void executeCommand(DirectoryHandle* dh, char* command, char* argument){
  int res;
  if(strncmp(command, "cd", 2) == 0) {
    res = SimpleFS_changeDir(dh, argument);
    if(res != 0){
      printf("Error in cd\n");
    }
  }
  else if(strncmp(command, "mkdir", 5) == 0){
    res = SimpleFS_mkDir(dh, argument);
    if(res != 0){
      printf("Error in mkdir\n");
    }
  }
  else if(strncmp(command, "touch", 5) == 0){
    FileHandle* fh = SimpleFS_createFile(dh, argument);
    if(fh == NULL){
      printf("Error in createFile\n");
    }
    free(fh);
  }
  else if(strncmp(command, "rmfile", 6) == 0){
    res =  SimpleFS_rmFile(dh, argument);
    if(res != 0){
      printf("Error in rmfile\n");
    }
  }
  else if(strncmp(command, "rmdir", 5) == 0){
    res = SimpleFS_changeDir(dh, argument);
    if(res != 0){
      printf("Error in cd\n");
    }
    res =  SimpleFS_rmDir(dh);
    if(res != 0){
      printf("Error in rmdir\n");
    }
  }
  else if(strncmp(command, "write", 5) == 0){
    char toWrite[MAXBUFFER];
    printf("Waiting for input (no spaces pls):\n");
    scanf("%s", toWrite);
    FileHandle* fh = SimpleFS_openFile(dh, argument);
    res = SimpleFS_write(fh, toWrite, strlen(toWrite));
    if(res != strlen(toWrite)){
      printf("Error while writing in file\n");
    }
    res = SimpleFS_close(fh);
    if(res != 0) printf("Error while closing file\n");
  }
  else if(strncmp(command, "read", 4) == 0){
    FileHandle* fh = SimpleFS_openFile(dh, argument);
    char readInto[MAXBUFFER];
    res = SimpleFS_read(fh, readInto, MAXBUFFER);
    if(res < 0) printf("Error while reading file\n");
    int i;
    for(i = 0; i < res; i++) printf("%c", readInto[i]);
    printf("\n");
    res = SimpleFS_close(fh);
    if(res != 0) printf("Error while closing file\n");
  }
  else printf("Command not recognized, try again\n");
  return;
}

int main(int argc, char** argv){
  //printf("Initializing filesystem\n");
  //Creating stuff
  SimpleFS* fs = (SimpleFS*) malloc(sizeof(SimpleFS));
  DiskDriver* disk = (DiskDriver*) malloc(sizeof(DiskDriver));
  DiskDriver_init(disk, 512);
  DirectoryHandle* root = SimpleFS_init(fs, disk);
  if(root == NULL){
    printf("Error while creating fileSystem\n");
    return 1;
  }
  DirectoryHandle* dh = cloneDh(root);
  //end creating stuff
  char command[MAX_INPUT];
  char argument[MAX_INPUT];
  print_help();
  while(1){     //starting terminal-like behaviour
    printf("SimpleFS>> ");
    scanf("%s", command);
    if(strncmp(command, "quit", 4) == 0) {
      SimpleFS_rmslash(root);
      break;
    }
    else if(strncmp(command, "ls", 2) == 0){
      SimpleFS_ls(dh);
    }
    else if(strncmp(command, "help", 4) == 0){
      print_help();
    }
    else{
    scanf("%s", argument);
    executeCommand(dh, command, argument);
    }
  }
  return 0;
}
