#include <stdio.h>
#include "./fileSystem.h"
#define NUM_BLOCKS 512
#define FILES_CREATED 3
#define NUM_TRIES 2

int main(int argc, char const *argv[]) {

  int res; //Error management
  SimpleFS* fs = (SimpleFS*) malloc(sizeof(SimpleFS));
  DiskDriver* disk = (DiskDriver*) malloc(sizeof(DiskDriver));
  DiskDriver_init(disk, NUM_BLOCKS);
  DirectoryHandle* rootHandler = SimpleFS_init(fs, disk);   //From now on, May God be with us all
  if(rootHandler != NULL) {
    printf("Amazing you got 'til here'\n");
  }

  //Debugging print follows
  print_info_dh(rootHandler);
  printf("%lu\n", sizeof(rootHandler -> dcb -> num_entries));

  char name[] = "Users";

  FileHandle* fh = SimpleFS_createFile(rootHandler, name);
  if(fh == NULL){
    printf("Something went off while creating file\n");
    return 1;
  }
  print_info_fh(fh);

  fh = SimpleFS_createFile(rootHandler, name);
  if(fh == NULL) printf("File already exists, all clear\n");
  else printf("Something went off while checking for duplicate files\n");

  char name2[] = "Kebab";
  FileHandle* fh2 = SimpleFS_createFile(rootHandler, name2);
  print_info_fh(fh2);

  char name3[] = "Doc.txt";
  FileHandle* fh3 = SimpleFS_createFile(rootHandler, name3);
  print_info_fh(fh3);

  char** names = (char**) malloc(sizeof(char*)*FILES_CREATED);
  res = SimpleFS_readDir(names, rootHandler);

  print_info_dh(rootHandler);

  if(res != 0) {
    printf("Something went wrong with readDir\n");
    return 1;
  }
  int i;
  printf("Files: ");
  for(i = 0; i < FILES_CREATED; i++)
    printf("%s\t", names[i]);         //and not names[1], names[2] ... damn programming at 2 am

  printf("\n");

  FileHandle* openedFile = SimpleFS_openFile(rootHandler, names[0]);
  if(openedFile == NULL) {
    printf("Something went wrong while opening file\n");
    return 1;
  }
  print_info_fh(openedFile);

  printf("Closing %s file\n", names[0]);
  res = SimpleFS_close(openedFile);
  if(res != 0){
    printf("Something went off while closing file\n");
    return 1;
  }
  printf("Done\n");

  FileHandle* openedFileAgain = SimpleFS_openFile(rootHandler, names[0]);
  if(openedFileAgain == NULL) {
    printf("Something went wrong while opening file\n");
    return 1;
  }
  print_info_fh(openedFileAgain);
  printf("Will try to write something in %s file\n", names[0]);
  char data[] = "Leonardo";       //sample data

  int written = SimpleFS_write(openedFileAgain, data, strlen(data));
  printf("%d byte were written in file\n", written);

  print_info_dh(rootHandler);

  print_info_fh(openedFileAgain);
  written = 0;
  //will try something silly to check how it goes
  //Let's write data in file lots of times


  int j;
  for(j = 0; j < NUM_TRIES; j++){
    written += SimpleFS_write(openedFileAgain, data, strlen(data));
  }
  printf("%d total byte were written, should be %d\n", written, (int)strlen(data)*NUM_TRIES);
  print_info_fh(openedFileAgain);

  printf("Tryin' reading from file\n");
  char* toRead = (char*) malloc(sizeof(char)* 8);
  res = SimpleFS_seek(openedFileAgain, 0);
  if(res == -1){
    printf("Something went off with seek\n");
    return 1;
  }

  res = SimpleFS_read(openedFileAgain, toRead, 8);
  printf("%d bytes have been read, should be %d\n", res, 8);
  printf("Read: %s\n", toRead);
  //good measure
  SimpleFS_close(openedFileAgain);

  char nameDir[] = "Users";
  res = SimpleFS_mkDir(rootHandler, nameDir);
  if(res != 0){
    printf("Something occured while creating directory\n");
    return 1;
  }
  printf("%s directory created\n", nameDir);
  printf("Before cd\n");
  print_info_dh(rootHandler);
  res = SimpleFS_changeDir(rootHandler, "Ciaone");
  if(res != 1){
    printf("Error in changeDir\n");
    return 1;
  }
  printf("Cd didn't change dir, no dir named Ciaone found\n");
  res = SimpleFS_changeDir(rootHandler, nameDir);
  if(res != 0){
    printf("Error in changeDir\n");
    return 1;
  }
  printf("After cd\n");
  print_info_dh(rootHandler);

  printf("Creating a second directory in root\n");
  res = SimpleFS_changeDir(rootHandler, "..");
  if(res != 0){
    printf("Error in changeDir\n");
    return 1;
  }
  res = SimpleFS_mkDir(rootHandler, "Application");
  if(res != 0){
    printf("Something occured while creating directory\n");
    return 1;
  }
  printf("All clear\n");
  print_info_dh(rootHandler);
  res = SimpleFS_changeDir(rootHandler, "Application");
  if(res != 0){
    printf("Error in changeDir\n");
    return 1;
  }
  print_info_dh(rootHandler);
  return 0;
}
