#include <stdio.h>
#include "./fileSystem.h"
#define NUM_BLOCKS 512
#define FILES_CREATED 3
#define NUM_TRIES 2

int main(int argc, char const *argv[]) {

  int res; //Error Managment
  SimpleFS* fs = (SimpleFS*) malloc(sizeof(SimpleFS));
  DiskDriver* disk = (DiskDriver*) malloc(sizeof(DiskDriver));
  DiskDriver_init(disk, NUM_BLOCKS);
  DirectoryHandle* rootHandler = SimpleFS_init(fs, disk);   //From now on, May God be with us all
  if(rootHandler != NULL) {
    printf("Amazing you got 'til here'\n");
  }

  res = SimpleFS_mkDir(rootHandler, "test");
  if(res != 0) return 1;

  res = SimpleFS_changeDir(rootHandler, "test");
  if(res != 0) return 1;

  FileHandle* created = SimpleFS_createFile(rootHandler, "Document.txt");
  if(created == NULL) return 1;
  SimpleFS_close(created);

  created = SimpleFS_createFile(rootHandler, "SO.docx");
  if(created == NULL) return 1;
  SimpleFS_close(created);

  created = SimpleFS_createFile(rootHandler, "helloWorld.c");
  if(created == NULL) return 1;
  SimpleFS_close(created);

  created = SimpleFS_createFile(rootHandler, "test3.exe");
  if(res != 0) return 1;

  FileHandle* fh = SimpleFS_createFile(rootHandler, "Ciao.txt");
  if(fh == NULL) return 1;

  res = SimpleFS_mkDir(rootHandler, "Dir");
  if(res != 0) return 1;

  res = SimpleFS_changeDir(rootHandler, "Dir");
  if(res != 0) return 1;

  res = SimpleFS_rmDir(rootHandler);
  if(res != 0) return 1;
  SimpleFS_ls(rootHandler);

  //brace yourselves
  res = SimpleFS_rmslash(rootHandler);
  if(res != 0) return 1;

  return 0;
}
