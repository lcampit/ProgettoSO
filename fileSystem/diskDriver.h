#pragma once
#include "../allocator/bit_map.h"
#include "fileSystem.h"

#define BLOCK_SIZE 512
#define NUM_BLOCK 32

typedef struct {
  int num_blocks;        // how many blocks are in the bitmap
  int bitmap_entries;    // how many bytes are needed to store the bitmap

  int free_blocks;       // number of free blocks
  int first_free_block;  // first free block index
} DiskHeader;

typedef struct {
  DiskHeader* header;
  BitMap* bitmap;
  FirstDirectoryBlock* mem;
} DiskDriver;


// FF & MG: allocates the necessary space on the disk,
// fills in the bitmap of appropriate size
// with all 0 (to denote the free space), and
// creates the root directory with its FCB
void DiskDriver_init(DiskDriver* disk, int num_blocks, buffer* buf);

// FF & MG: reads the block in position block_num
// returns -1 if the block is free according to the bitmap
// 0 otherwise
int DiskDriver_readBlock(DiskDriver* disk, void* dest, int block_num);

// FF & MG: writes/overwrites a block in position block_num, and alters the bitmap accordingly
// returns -1 if block_num exceeds the max number of blocks
// or if the block I want to write on is the root
int DiskDriver_writeBlock(DiskDriver* disk, void* src, int block_num);

// FF & MG: frees a block in position block_num, and alters the bitmap accordingly
// returns -1 if block_num exceeds the max number of blocks
// or if the block I want to free is the root
int DiskDriver_freeBlock(DiskDriver* disk, int block_num);

// FF & MG: returns the first free block in the disk from position (checking the bitmap)
int DiskDriver_getFreeBlock(DiskDriver* disk, int start);
