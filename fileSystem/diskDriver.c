#include "diskDriver.h"

void DiskDriver_init(DiskDriver* disk, int num_blocks, buffer* buf) {
  FirstDirectoryBlock* memo = my_alloc(buf, sizeof(FirstDirectoryBlock)*num_blocks*BLOCK_SIZE);
  disk->mem=memo;

  BitMap bit;
  BitMap_init(&bit, NUM_BLOCK);

  DiskHeader* head = my_alloc(buf, sizeof(DiskHeader));
  head->num_blocks = NUM_BLOCK;
  head->bitmap_entries = NUM_BLOCK/8;
  head->free_blocks = NUM_BLOCK;
  head->first_free_block = 1;

  FirstDirectoryBlock* root = my_alloc(buf, sizeof(FirstDirectoryBlock));   //allocation of first directory "/"
  root->num_entries = 0;
  int* a = my_alloc(buf, (BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int))/sizeof(int));
  root-> file_blocks = a;
  BlockHeader* blockh = my_alloc(buf, sizeof(BlockHeader));
  blockh->previous_block = -1;
  blockh->next_block = -1
  blockh->block_in_file = 0;
  root->header = blockh;
  FileControlBlock* ficb = my_alloc(buf, sizeof(FileControlBlock));
  ficb->directory_block=-1;
  ficb->block_in_disk=0;
  ficb->name="/";
  ficb->size_in_bytes = sizeof(FirstDirectoryBlock);
  ficb->size_in_blocks=1;
  ficb->is_dir=1;
  root->fcb=ficb;
  disk->mem[0] = root;
  return;
}

int DiskDriver_readBlock(DiskDriver* disk, void* dest, int block_num){
  *dest = disk->mem[block_num];
  int check = BitMap_get(disk->bitmap, block_num);
  if (check==0) return -1;
  else return 0;
}

int DiskDriver_writeBlock(DiskDriver* disk, void* src, int block_num){
  if (block_num>NUM_BLOCK || block_num==0) return -1;
  if (disk->mem[block_num]==NULL) disk->header->free_blocks-=1;
  disk->mem[block_num]=src;
  BitMap_set(disk->bitmap, block_num);
  if (disk->header->first_free_block==block_num) disk->header->first_free_block=block_num+1;
  return 0;
}

int DiskDriver_freeBlock(DiskDriver* disk, int block_num) {
  if (block_num>NUM_BLOCK || block_num==0) return -1;
  if (disk->mem[block_num]!=NULL) disk->header->free_blocks+=1;
  disk->mem[block_num] = NULL;
  BitMap_unset(disk->bitmap, block_num);
  if (disk->header->first_free_block>block_num) disk->header->first_free_block=block_num;
  return 0;
}

int DiskDriver_getFreeBlock(DiskDriver* disk, int start){
  if (start>disk->header->first_free_block) return start;
  else return disk->header->first_free_block;
}
