#include "includerOne.h"


DirectoryHandle* SimpleFS_init(SimpleFS* fs, DiskDriver* disk){
  //Fs già allocato
  fs -> buf = buffer_init(FS_SIZE);   //Prolly matrioska effect LOL
  fs -> disk = disk;

  DirectoryHandle res;
  res.sfs = fs;
  res.dcb = disk -> mem[0];
  res.directory = NULL;

  res.current_block = res.dcb -> header;

  res.pos_in_dir = 0;
  res.pos_in_file = 0;

  return &res;
}

FileHandle* SimpleFS_createFile(DirectoryHandle* d, const char* filename){

  FirstDirectoryBlock* blocks = d -> sfs -> disk -> mem;
  FirstDirectoryBlock* dcb = blocks -> dcb;
  int i;
  for(i = 0; i < dcb -> num_entries; i++){
    FirstFileBlock* fb = (FirstFileBlock*) blocks[dcb->file_blocks[i]];
    if(strcmp(filename, fb -> fcb -> name) == 0){
      return NULL;
    }
  }

  FirstFileBlock* newFile = my_alloc(d -> fs -> buf, sizeof(FirstFileBlock));
  newFile -> header -> previous_block = -1;
  newFile -> header -> next_block = -1;
  newFile -> header -> block_in_file = 1;

  newFile -> fcb -> directory_block = d -> dcb -> fcb -> block_in_disk;
  newFile -> fcb -> block_in_disk = DiskDriver_getFreeBlock(d -> fs -> disk, 0);
  newFile -> fcb -> name = filename;
  newFile -> fcb -> size_in_bytes = 0;
  newFile -> fcb -> size_in_blocks = 1;
  newFile -> fcb -> is_dir = 0;

  int res = DiskDriver_writeBlock(d -> sfs -> disk, newFile, newFile -> fcb -> block_in_disk);
  if(res != 0) return NULL;
  d -> dcb -> file_blocks[d -> num_entries+1] = newFile -> fcb -> block_in_disk;
  d -> num_entries += 1;

  FileHandle* to_return = my_alloc(d -> sfs -> buf, sizeof(FileHandle));
  to_return -> sfs = d -> sfs;
  to_return -> fcb = newFile;
  to_return -> current_block = newFile -> header;
  to_return -> pos_in_file = 0;
  to_return -> directory = d -> dcb;
  return to_return;
  //DiskDriver_writeBlock(d -> fs -> disk, newFile, newFile -> block_in_disk);
}

int SimpleFS_readDir(char** names, DirectoryHandle* d){
  FirstDirectoryBlock* blocks = d -> sfs -> disk -> mem;
  FirstDirectoryBlock* dcb = blocks -> dcb;
  int i;
  for(i = 0; i < dcb -> num_entries; i++){
    FirstFileBlock* fb = (FirstFileBlock*) blocks[dcb->file_blocks[i]];
    FileControlBlock* fcb =  fb -> fcb;
    names[i] = fcb -> name;
  }
  return i;
}

FileHandle* SimpleFS_openFile(DirectoryHandle* d, const char* filename){
  FirstDirectoryBlock* blocks = d -> sfs -> disk -> mem;
  FirstDirectoryBlock* dcb = blocks -> dcb;
  int i;
  for(i = 0; i < dcb -> num_entries; i++){
    FirstFileBlock* fb = (FirstFileBlock*) blocks[dcb->file_blocks[i]];
    if(strcmp(filename, fb -> fcb -> name) == 0){
      FileHandle* to_ret = my_alloc(d -> sfs -> buf, sizeof(FileHandle));
      to_ret -> sfs =  d -> sfs;
      to_ret -> fcb = fb -> fcb;
      to_ret -> directory = d -> dcb;
      to_ret -> current_block = fb -> header;
      to_ret -> pos_in_file = fb -> fcb -> size_in_bytes;
      return to_ret;
    }
  }
  return NULL; //Shit hit the fan
}

int SimpleFS_close(FileHandle* f){
  my_free(f);
  return 0;
}

// writes in the file, at current position for size bytes stored in data
// overwriting and allocating new space if necessary
// returns the number of bytes written
int SimpleFS_write(FileHandle* f, void* data, int size){
  int start =  f -> pos_in_file;
  int jumps = f -> current_block -> block_in_file;
  FirstFileBlock* first = f -> fcb;
  FileBlock* w = first;
  for(j = 0; j < jumps; j++){
    int next = w -> header -> next_block;
    w = f -> sfs -> disk -> mem[next];
  }
  if(jumps == 0){ //writes in first

  }

  else{   //writes in w

  }


  for(i = start; i < size; i++){
    if(i > BLOCK_SIZE){ //Creates a new Block

    }
    first -> data[i] = data[i];
    first -> fcb -> size_in_bytes++;
    f -> pos_in_file ++;
  }
}
