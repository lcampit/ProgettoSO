#include <stdio.h>
#include "diskDriver.h"
//How many blocks of block size the disk should old.
//Muswt be a multiple of 8 (to avoid disasters)
#define NUM 512


int main(){
  DiskDriver* disk = (DiskDriver*) malloc(sizeof(DiskDriver));
  DiskDriver_init(disk, NUM);
  int res;

  int index = DiskDriver_getFreeBlock(disk, 4);
  printf("First Free Block found at index: %d\n", index);

  int data = 2;
  void* p = &data;

  printf("Data: %d\n", data);
  printf("Writing data in block: %d\n", index);
  res = DiskDriver_writeBlock(disk, p, index);
  printf("Done, result: %d\n", res);

  void* result;
  printf("Retrieving data from block\n");
  res = DiskDriver_readBlock(disk, &result, index);
  printf("Done, result: %d\n", res);
  printf("Value read: %d\n", *(int*)result);

  printf("Writing data in block: %d\n", index-1);
  res = DiskDriver_writeBlock(disk, p, index-1);
  printf("Done, result: %d\n", res);

  printf("Writing data in block: %d\n", index+3);
  res = DiskDriver_writeBlock(disk, p, index+3);
  printf("Done, result: %d\n", res);

  printf("Printing BitMap status\n");
  BitMap_print(disk->map, 0, 10);

  printf("Beginning Freeing blocks\n");
  DiskDriver_freeBlock(disk, index);
  DiskDriver_freeBlock(disk, index-1);
  DiskDriver_freeBlock(disk, index+3);
  printf("Printing BitMap Status\n");
  BitMap_print(disk->map, 0, 10);

  printf("Checking for error management\n");
  int check1 = 0, check2 = 0, check3 = 0, check4 = 0, check5 = 0, check6 = 0, check7 = 0;
  check1 += DiskDriver_freeBlock(disk, index);                        //Tries freeing an empty block, returns 0
  check2 += DiskDriver_freeBlock(disk, index+NUM);                    //Tries freeing an out of bounds block, returns 1
  check3 += DiskDriver_readBlock(disk, &result, index-1);             //Tries reading an empty block, returns 0
  check4 += DiskDriver_readBlock(disk, &result, index+NUM);           //Tries reading from an out of bounds block, returns 0
  check5 += DiskDriver_getFreeBlock(disk, index+NUM);                 //Tries retrieving a free block from out of bounds, returns -1
  DiskDriver_writeBlock(disk, p, index);                              //Writing on a block for next test
  check6 += DiskDriver_writeBlock(disk, p, index);                    //Tries writing in a non free block, returns 1
  check7 += DiskDriver_writeBlock(disk, p, index+NUM);                //Tries writing in an out of bounds block, returns 1

  if(check1+check2+check3+check4+check5+check6+check7 == 2){
    printf("Everything is working as inteded, congrats!\n");
  }
  else {
    printf("Something went off\n");
    printf("FreeBlock on empty block, result: %d\n", check1);
    printf("FreeBlock on out of bounds block, result: %d\n", check2);
    printf("ReadBlock on empty block, result: %d\n", check3);
    printf("ReadBlock on out of bounds block, result: %d\n", check4);
    printf("GetFreeBlock on out of bounds block, result: %d\n", check5);
    printf("WriteBlock on non free block, result: %d\n", check6);
    printf("WriteBlock on out of bounds block, result: %d\n", check7);
  }

  printf("Printing info on BitMap\n");
  BitMap_printInfo(disk -> map);
  printf("Disk got %d blocks, should be 1 bit per block\n", NUM);
  return 0;
}
