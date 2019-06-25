#pragma once
#include "bit_map.h"
#include <stdlib.h>       //For malloc and free, will be using our allocator once evrything is set up
#include <stdio.h>        //Debugging the ol' reliable way
#include <string.h>

#define BLOCK_SIZE 512

//Disk contains blocks of BLOCK_SIZE size, which will be casted
//to the requested types during request from fileSystem

//Each block of num_block is made by BLOCK_SIZE pointers
//selecting the pointer inside the block makes the cursor move and other stuff

typedef struct {
  int num_blocks;      // total number of blocks in disk
  int bitmap_blocks;   // blocks in bitmap
  int bitmap_entries;  // number of bytes in bitmap

  int free_blocks;     // number of free blocks
  int first_free_block;// first free block index
} DiskHeader;

typedef struct {
  DiskHeader* header; // first block of the disk
  BitMap* map;        // BitMap used for marking blocks
  void** blocks;       // Blocks of Disk
} DiskDriver;

//LC
//creates all needed structs for all operations, allocating
//the necessary memory, side-effecting on the preallocated disk pointer
//creates a diskDriver of num_blocks blocks
//the bitmap has a block for each disk driver block
void DiskDriver_init(DiskDriver* disk, int num_blocks);

//LC
// writes in dest the block read in the dist at block_num position
// returns 1 if block read was written, 0 otherwise
int DiskDriver_readBlock(DiskDriver* disk, void** dest, int block_num);

//LC
// write in disk, position block_num, the data in src.
// updates the bitmap accordingly
// returns 0 if everything goes well, 1 otherwise
int DiskDriver_writeBlock(DiskDriver* disk, void* src, int block_num);

// LC
// Empties a block in the disk at block_num position
// Updates the bitmap accordingly
// returns 0 if everything goes well, 1 otherwise
int DiskDriver_freeBlock(DiskDriver* disk, int block_num);

// LC
// returns an index for the first free block in the bitmap
// returns -1 if there aren't any
int DiskDriver_getFreeBlock(DiskDriver* disk, int start);
