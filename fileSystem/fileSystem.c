#include "./fileSystem.h"


// LC
// initializes fileSystem on given disk (both preallocated)
// Creates the root directory and saves the block in first position on disk
// Returns a DirectoryHandle for root dir
// Returns NULL if anything wrong happens
DirectoryHandle* SimpleFS_init(SimpleFS* fs, DiskDriver* disk){
  //initializing fileSystem, linking disk
  fs -> disk = disk;
  //Creating root
  FirstDirectoryBlock* root = (FirstDirectoryBlock*) malloc(sizeof(FirstDirectoryBlock));
  root -> header.previous_block = 0;//no previous block for root
  root -> header.next_block = 0;    //no more blocks, for now
  root -> header.block_in_file = 0; //Block contains a fcb

  root -> fcb.parent_directory = 0; //No parent for root, poor thing
  root -> fcb.block_in_disk = 0;    //empty dir
  root -> fcb.name[0] = '/';        //Name of root directory
  root -> fcb.name[1] = '\0';       //null-terminated string
  root -> fcb.size_in_bytes = 0;    //empty dir
  root -> fcb.size_in_blocks = 1;   //empty dir
  root -> fcb.is_dir = 1; //yes, it is

  root -> num_entries = 0; //For now, just creating things, will be updated as time goes
  //All set, writing block to disk
  int res = DiskDriver_writeBlock(disk, root, 0);
  if(res != 0){         //Error management
    printf("Critical error, somtething went off while writing root block on disk\n");
    return NULL;
  }

  //Creating a DirectoryHandle for Root dir
  DirectoryHandle* dh = (DirectoryHandle*) malloc(sizeof(DirectoryHandle));
  dh -> sfs = fs;   //Linking fileSystem

  res = DiskDriver_readBlock(fs -> disk, (void**)&(dh->dcb), 0);      //reading root block
  if(res != 1){
    printf("Something went off while reading root block");
    return NULL;
  }
  dh -> directory = NULL;     //top level, no need for this
  dh -> current_block = (BlockHeader*) malloc(sizeof(BlockHeader));   //placeholder
  dh -> current_block -> previous_block = 0;          //root level, no previous block
  dh -> current_block -> block_in_file = 0;           //First block of dir, contains a fcb
  dh -> current_block -> next_block = 0;              //initializing, no next block as of now

  dh -> pos_in_dir = 0;
  dh -> pos_in_block = 0;   //position of cursor in block

  return dh;                //Hopefully we get here, one way or another
}


// LC
// creates an empty first file block in directorty dir with name filename
// returns NULL if anything happens (no free space, file existing)
FileHandle* SimpleFS_createFile(DirectoryHandle* dir, const char* filename){

  int nextFreeBlock = DiskDriver_getFreeBlock(dir -> sfs -> disk, dir -> dcb -> fcb . block_in_disk);
  if(nextFreeBlock == -1){
    printf("No space left on disk\n");
    return NULL;
  }

  int max = sizeof(dir -> dcb -> num_entries); //computes maximum block capacity of dir
  if(dir -> pos_in_dir > max){
    printf("No space left for file in current Directory Block\n");
    return NULL;
  }

  //checking for same-name files

  //in FirstDirectoryBlock
  FirstDirectoryBlock* fdb = dir -> dcb;
  FileBlock* dest = (FileBlock*) malloc(sizeof(FileBlock));
  int i, res;
  for(i = 0; i < fdb -> num_entries; i++){
    //retrieves file block
    int index = fdb -> file_blocks[i];
    res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(dest), index);
    if(res != 1){
      printf("Dir Block was not written on disk\n");
      return NULL;
    }
    if(dest -> header . block_in_file == 0){   //we got a first file Block with a FCB, can safely cast
      FirstFileBlock* casted = (FirstFileBlock*) dest;
      if(strncmp(filename, casted -> fcb.name, strlen(filename)) == 0){   //Same name file exits
        printf("File already exists\n");
        return NULL;
      }
    }
  }

  /**** BEGIN CARE ****/
  //This code checks if there is a file with the same name in any directory block linked
  //to the provided one via his header.
  //Not sure if it's needed, so it's commented
  //If this comes up to be useful, it may require some additional attention
  /**** END CARE ****/

  /*
  DirectoryBlock* nextDirBlock = (DirectoryBlock*) fdb;
  while(nextDirBlock->header.next_block != 0){      //'til there are next dir block
    res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(nextDirBlock), nextDirBlock->header.next_block);
    if(res != 1){
      printf("Next Dir block was not written on disk\n");
      return NULL;
    }
    for(i = 0; i < nextDirBlock -> num_entries; i++){
      //retrieves file block
      int index = fdb -> file_blocks[i];
      res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(dest), index);
      if(res != 1){
        printf("Dir Block was not written on disk\n");
        return NULL;
      }
      if(dest -> header . block_in_file == 0){   //we got a first file Block with a FCB, can safely cast
        FirstFileBlock* casted = (FirstFileBlock*) dest;
        if(strncmp(filename, casted -> fcb.name, strlen(filename)) == 0){   //Same name file exits
          printf("File already exists\n");
          return NULL;
        }
      }
    }
  }

  */


  //If we got here, no file with same name was found inside this Directory
  //We'll proceed creating the empty First File Block

  FirstFileBlock* newFile = (FirstFileBlock*) malloc(sizeof(FirstFileBlock));
  FileHandle* fh = (FileHandle*) malloc(sizeof(FileHandle));

  //Begin creating new File
  newFile->header.previous_block =  0;
  newFile->header.next_block = 0;
  newFile->header.block_in_file = 0;

  newFile->fcb.parent_directory = dir -> dcb->fcb.block_in_disk;   //Index to the first block of the directory containing the file
  newFile->fcb.block_in_disk = nextFreeBlock;
  strncpy(newFile->fcb.name, filename, strlen(filename));
  newFile->fcb.size_in_bytes = 0;
  newFile->fcb.size_in_blocks = 1;
  newFile->fcb.is_dir = 0;      //No it's not.

  //End creating new file

  //Writign block on disk
  res = DiskDriver_writeBlock(dir -> sfs -> disk, newFile, nextFreeBlock);
  if(res != 0){
    printf("Error while writing block on disk");
    return NULL;
  }
  //Begin creating File Handle

  fh -> sfs = dir -> sfs;
  fh -> fcb = newFile;
  fh -> directory = dir -> dcb;
  fh -> current_block = &newFile -> header;
  fh -> pos_in_file = 0;        //cursor at start of file

  //End creating new file

  free(dest); //releasing resources

  //updating directory values

  dir -> dcb -> file_blocks[dir->dcb->num_entries] = nextFreeBlock;
  dir -> dcb -> num_entries++;


  return fh; //With God Help, we should be able to get 'til here

}

// LC
// writes in the preallocated blocks array the names of all files in dir
// returns 0 if anything is ok, 1 otherwise
int SimpleFS_readDir(char** names, DirectoryHandle* dir){
  int max = dir -> dcb -> num_entries;
  int i, res;
  if(max == 0) return 1;              //empty dir;
  FirstFileBlock* dest = (FirstFileBlock*) malloc(sizeof(FirstFileBlock));
  for(i = 0; i < max; i++){
    int index = dir -> dcb -> file_blocks[i];
    res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(dest), index);   //retrieves a fileblock

    if(res != 1) {
      printf("Block was not written on disk\n");
      return 1;
    }
    char* src = dest -> fcb.name;
    names[i] = src;

    //int len = strlen(src);                They segFault, idk why
    //strncpy(names[i], src, len);
  }
  return 0;
}


//****************** DEBUGGING FUNCTIONS **********************//
// LC
// prints info about DirectoryHandle values, checks if anything is correct
void print_info_dh(DirectoryHandle* dh){
  printf("DirectoryHandle relative to %s directory\n", dh -> dcb->fcb.name);
  printf("Dcb values: \n\tparent_directory: %d,\tblock_in_disk: %d\n\tsize_in_bytes: %d,\tsize_in_blocks: %d,\tis_dir: %d\n", //Pretty lame i know
          dh ->dcb->fcb.parent_directory,dh ->dcb->fcb.block_in_disk, dh ->dcb->fcb.size_in_bytes, dh ->dcb->fcb.size_in_blocks, dh ->dcb->fcb.is_dir);

  print_info_block_header(&(dh -> dcb->header));
  printf("Num_entries: %d\n", dh -> dcb -> num_entries);

  printf("Printing current header\n");
  print_info_block_header(dh->current_block);

  printf("Final Infos on DirectoryHandle\n");
  printf("Pos_in_dir: %d\tpos_in_block: %d\n", dh -> pos_in_dir, dh -> pos_in_block);

  return;
}

// LC
// prints info about blockHeader values, checks if anything is correct
void print_info_block_header(BlockHeader* bh){
  printf("Header: prev %d\tnext %d\tblock_in_file %d\n", bh -> previous_block, bh -> next_block, bh -> block_in_file);
  return;
}


// LC
// prints info about fileHandle values, checks if everything is ok
void print_info_fh(FileHandle* fh){
  printf("FileHandle relative to %s file\n", fh -> fcb -> fcb.name);
  printf("Fcb values: \n\tparent_directory: %d,\tblock_in_disk: %d\n\tsize_in_bytes: %d,\tsize_in_blocks: %d,\tis_dir: %d\n", //Pretty lame i know
          fh ->fcb->fcb.parent_directory,fh ->fcb->fcb.block_in_disk, fh ->fcb->fcb.size_in_bytes, fh ->fcb->fcb.size_in_blocks, fh ->fcb->fcb.is_dir);
  print_info_block_header(&(fh -> fcb->header));
  printf("Pos_in_file: %d\n", fh -> pos_in_file);
  return;
}
