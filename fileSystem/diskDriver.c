#include "./diskDriver.h"

//LC
//creates all needed structs for all operations, allocating
//the necessary memory, side-effecting on the provided disk pointer
//creates a diskDriver of num_blocks blocks
//the bitmap has a bit for each disk driver block
void DiskDriver_init(DiskDriver* disk, int num_blocks){
    disk -> header -> num_blocks = num_blocks;
    disk -> header -> bitmap_blocks = num_blocks;
    disk -> header -> bitmap_entries = num_blocks / 8;      //number of bytes in the bitmap
    disk -> header -> free_blocks = num_blocks;
    disk -> header -> first_free_block = 0;
    BitMap_init(disk->map, disk -> header -> bitmap_entries); //creates bitmap with bitmpa_entries * 8 bit, one bit per block

    //Using malloc for testing, will switch to our allocator once everything is done
    disk -> blocks = (char*) malloc(sizeof(char)* num_blocks * BLOCK_SIZE);
    return;
}



//LC
// writes in dest the block read in the dist at block_num position
// returns 1 if block read was written, 0 elsewhere
int DiskDriver_readBlock(DiskDriver* disk, char* dest, int block_num){
  int resBit = BitMap_get(disk->map, block_num);
  char retrieved = disk -> blocks[block_num];
  *dest = retrieved;
  return resBit;        //dest is valid only if block was written
}

//LC
// write in disk, position block_num, the data in src.
// updates the bitmap accordingly
// returns 0 if everything goes well, 1 otherwise
// TODO error cases, when to retun 1?
int DiskDriver_writeBlock(DiskDriver* disk, char* src, int block_num){
  BitMap_set(disk -> map, block_num);
  disk -> blocks[block_num] = *src;
  return 0;
}

// LC
// Empties a block in the disk at block_num position
// Updates the bitmap accordingly
// returns 0 if everything goes well, 1 otherwise
int DiskDriver_freeBlock(DiskDriver* disk, int block_num){
  if(BitMap_get(disk -> map, block_num) == 0) { //Should not free an empty block
    return 1;
  }
  else {
    BitMap_unset(disk -> map, block_num); //Set block_num bit to 0, free block
    disk -> blocks[block_num] = 0;        //Empties block
    return 0;
  }
}

// LC
// returns an index for the first free block in the bitmap
// returns -1 if there aren't any
int DiskDriver_getFreeBlock(DiskDriver* disk, int start){
  int i;
  for(i = start; i < disk -> header -> bitmap_blocks; i++){
    if(BitMap_get(disk -> map, i) == 0){
      return i;       //Stops at first free block in bitmap
    }
  }
  return -1;          //Returns -1 if memory is full, no free blocks available
}
