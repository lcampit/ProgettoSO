#include <stdio.h>
#include "./fileSystem.h"
#define NUM_BLOCKS 512
#define FILES_CREATED 3
#define NUM_TRIES 45

int main(int argc, char const *argv[]) {

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
    printf("Something went wrong while creating file");
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
  int res = SimpleFS_readDir(names, rootHandler);

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
  //good measure
  SimpleFS_close(openedFileAgain);

  return 0;
}
