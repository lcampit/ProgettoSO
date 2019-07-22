#include "./fileSystem.h"

//TODO All memory leaks must be taken care of


// LC
// initializes fileSystem on given disk (both preallocated)
// Creates the root directory and saves the block in first position on disk
// Returns a DirectoryHandle for root dir
// Returns NULL if anything wrong happens
DirectoryHandle* SimpleFS_init(SimpleFS* fs, DiskDriver* disk){
  //initializing fileSystem, linking disk
  fs -> disk = disk;
  //Creating root
  FirstDirectoryBlock* root = (FirstDirectoryBlock*) malloc(sizeof(FirstDirectoryBlock));   //live long and prosper
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
    //printf("Critical error, somtething went off while writing root block on disk\n");
    return NULL;
  }

  //Creating a DirectoryHandle for Root dir
  DirectoryHandle* dh = (DirectoryHandle*) malloc(sizeof(DirectoryHandle));
  dh -> sfs = fs;   //Linking fileSystem

  res = DiskDriver_readBlock(fs -> disk, (void**)&(dh->dcb), 0);      //reading root block
  if(res != 1){
    //printf("Something went off while reading root block");
    return NULL;
  }
  dh -> directory = NULL;     //top level, no need for this
  dh -> current_block = (BlockHeader*) malloc(sizeof(BlockHeader));
  dh -> current_block -> previous_block = 0;          //root level, no previous block
  dh -> current_block -> block_in_file = 0;           //First block of dir, contains a fcb
  dh -> current_block -> next_block = 0;              //initializing, no next block as of now

  dh -> pos_in_dir = 0;
  dh -> pos_in_block = 0;   //position of cursor in block

  return dh;                //Hopefully we get here, one way or another
}


// LC
// creates an empty first file block in directorty dir with name filename
// if file exists, returns a file handle for the given file
// returns NULL if anything happens (no free space, etc)

FileHandle* SimpleFS_createFile(DirectoryHandle* dir, const char* filename){

  int nextFreeBlock = DiskDriver_getFreeBlock(dir -> sfs -> disk, dir -> dcb -> fcb . block_in_disk);
  if(nextFreeBlock == -1){
    printf("No space left on disk\n");
    return NULL;
  }
  int res;
  //Checks for same-name file
  char** names = (char**) malloc(sizeof(char*) * dir -> dcb -> num_entries);
  res = SimpleFS_readDir(names, dir, 0);
  if(res != 0){
    printf("Something went off while reading dir files names\n");
    return NULL; //Something occured with readDir
  }
  int i;
  for(i = 0; i < dir -> dcb -> num_entries; i++){
    if(strncmp(names[i], filename, strlen(filename)) == 0){   //same name file found
      //we'll open the file and return his fileHandle
      FileHandle* openedFile = SimpleFS_openFile(dir, filename);
      if(openedFile == NULL){
        return NULL;    //Something occured while opening file
      }
      return openedFile;
    }
  }
  //No same name file found, proceed with creation
  free(names);      //releasing resources

  FirstFileBlock* newFile = (FirstFileBlock*) malloc(sizeof(FirstFileBlock));

  //Begin creating new File
  newFile->header.previous_block =  0;
  newFile->header.next_block = 0;
  newFile->header.block_in_file = 0;      //contains an fcb

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
  FileHandle* fh = (FileHandle*) malloc(sizeof(FileHandle));

  fh -> sfs = dir -> sfs;
  fh -> fcb = newFile;
  fh -> directory = dir -> dcb;
  fh -> current_block = &newFile -> header;
  fh -> pos_in_file = 0;        //cursor at start of file

  //Will now check if the file can be stored in an existant (First)DirectoryBlock or a new DirectoryBlock must be created
  if(dir -> dcb -> num_entries < (BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int))/sizeof(int)){//Can safely write in file_blocks of FirstDirectoryBlock
    int h;
    for(h = 0; h < dir -> dcb -> num_entries; h++){
      if(dir -> dcb -> file_blocks[h] == FREE_BLOCK){
          dir -> dcb -> file_blocks[h] = nextFreeBlock;
          dir -> dcb -> num_entries ++;
          return fh;
      }
    }
    dir -> dcb -> file_blocks[dir -> dcb -> num_entries] = nextFreeBlock;
    dir -> dcb -> num_entries += 1;
    return fh;      //Everything's done
  }
  else {    //we must jump to the last DirectoryBlock, write in that block our file or create a new DirectoryBlock, linked with the last one, and write the file there
    int index = dir -> dcb -> num_entries - (BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int))/sizeof(int);
    DirectoryBlock* dest = (DirectoryBlock*) dir -> dcb;
    int num = 0 , previous_index = 0;
    while(dest -> header.next_block != 0){
      previous_index = dest -> header.next_block;
      res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(dest), dest -> header.next_block);   //Skips through DirectoryBlock
      if(res!= 1){      //DirectoryBlock was not written on disk, an error occured
        free(fh);
        DiskDriver_freeBlock(dir -> sfs -> disk, nextFreeBlock);
        return NULL;
      }
      int h;
      for(h = 0; h < (BLOCK_SIZE-sizeof(BlockHeader))/sizeof(int); h++){
        if(dest -> file_blocks[h] == FREE_BLOCK){ //previously deleted block was found, using that space
          dest -> file_blocks[h] = nextFreeBlock;
          dir -> dcb -> num_entries++;
          return fh;
        }
      }
      num++;
    }
    //If we are here, we found the last DirectoryBlock written on disk
    //We must check if the DirectoryBlock can contain another FirstFileBlock or if a new one must be created
    int relativeIndex = index - num*((BLOCK_SIZE-sizeof(BlockHeader))/sizeof(int));   //index relative to DirectoryBlock reached
    if(relativeIndex < (BLOCK_SIZE-sizeof(BlockHeader))/sizeof(int)){//we can safely add to dest our file
      int h;
      for(h = 0; h < relativeIndex; h++){
        if(dest -> file_blocks[h] == FREE_BLOCK){
          dest -> file_blocks[h] = nextFreeBlock;
          dir -> dcb -> num_entries++;
          return fh;
        }
      }
      dest -> file_blocks[relativeIndex] = nextFreeBlock;
      dir -> dcb -> num_entries++;
      return fh;
    }
    else{   //a new directory block must be created, linked with dest
      int blockForDir = DiskDriver_getFreeBlock(dir -> sfs -> disk, 0);   //fetches next free block in disk
      if(blockForDir == -1){
        //No free block available, aborting
        //releasing resources
        free(fh);
        DiskDriver_freeBlock(dir -> sfs -> disk, nextFreeBlock);
        return NULL;
      }
      //everything's clear, creating a DirectoryBlock
      DirectoryBlock* newDirBlock = (DirectoryBlock*) malloc(sizeof(DirectoryBlock));
      newDirBlock -> header . next_block = 0;
      newDirBlock -> header . previous_block = previous_index;
      newDirBlock -> header . block_in_file = dest -> header . block_in_file +1;

      //Write our file
      newDirBlock -> file_blocks[0] = nextFreeBlock;

      //Write DirectoryBlock on disk
      res = DiskDriver_writeBlock(dir -> sfs -> disk, newDirBlock, blockForDir);
      if(res != 0){
        //Something went wrong while writing directoryBlock on disk
        free(fh);
        DiskDriver_freeBlock(dir -> sfs -> disk, nextFreeBlock);
        DiskDriver_freeBlock(dir -> sfs -> disk, blockForDir);        //just to be sure
        return NULL;
      }

      dir -> dcb -> num_entries++;
      dir -> dcb -> fcb . size_in_blocks++;    //a new block was created
      dest -> header . next_block = blockForDir; //linking new block to chain
      return fh;
    }

  }
}


// LC
// writes in the preallocated blocks array the names of all files ,if flag
// equals 0, or all files and dirs if flag equals 1, in dir
// returns 0 if anything is ok, 1 otherwise
int SimpleFS_readDir(char** names, DirectoryHandle* dir, int flag){
  int max = dir -> dcb -> num_entries;
  int i, res, numFreeBlocks = 0;
  if(max == 0) return 0;              //empty dir;
  FirstFileBlock* dest = (FirstFileBlock*) malloc(sizeof(FirstFileBlock));
  for(i = 0; i < max; i++){
    if(i< (BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int))/sizeof(int) ){   //can still access in FirstDirectoryBlock
    int index = dir -> dcb -> file_blocks[i];
    //skips FreeBlock indexes
    if(index == FREE_BLOCK){
      max++;    //So dangerous it might just work
      numFreeBlocks++;
      continue;
    }
    res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(dest), index);   //retrieves a fileblock

    if(res != 1) {
      //printf("Block was not written on disk\n");
      return 1;
    }
    if(dest -> fcb . is_dir == 0) names[i - numFreeBlocks] = dest -> fcb . name;
    else if(flag == 0) continue;    ///skips dirs
    else names[i - numFreeBlocks] = dest -> fcb . name;
  }
  else break;
  }
    if(i < max){    //we must check in a DirectoryBlock
          int nextDirBlock = dir -> dcb -> header . next_block;
          DirectoryBlock dirBlock;
          DirectoryBlock* dirAux = &dirBlock;
          while(nextDirBlock != 0){
            res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(dirAux), nextDirBlock);
            nextDirBlock =  dirAux-> header . next_block;
            if(res != 1){
              //printf("Block was not written on disk\n");
              return 1;
            }
            for(; i < max; i++){
              if(i < (BLOCK_SIZE-sizeof(BlockHeader))/sizeof(int)){   //can still access in DirectoryBlock
              int index = dirAux -> file_blocks[i];
              if(index == FREE_BLOCK){
                max++;    //So dangerous it might just work
                numFreeBlocks++;
                continue;
              }
              res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(dest), index);   //retrieves a fileblock

              if(res != 1) {
                //printf("Block was not written on disk\n");
                return 1;
              }
              if(dest -> fcb . is_dir == 0) names[i - numFreeBlocks] = dest -> fcb . name;
              else if(flag == 0) continue;
              else names[i - numFreeBlocks] = dest -> fcb . name;
            }
            else break;     //skip to next DirectoryBlock
        }
      }
    }

    return 0;
}

// LC
// Creates a FileHandle about an existing file in the directory dir
// Returns NULL if anything happens
FileHandle* SimpleFS_openFile(DirectoryHandle* dir, const char* filename){
  //First we check if the provided file exists
  FirstDirectoryBlock* fdb = dir -> dcb;
  FirstFileBlock* dest = (FirstFileBlock*) malloc(sizeof(FileBlock));
  int i, res, max = fdb -> num_entries;
  for(i = 0; i < max; i++){
    //retrieves file block
    int index = fdb -> file_blocks[i];
    if(index == FREE_BLOCK){
      max++;
      continue;
    }
    res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(dest), index);
    if(res != 1){
      //printf("File Block was not written on disk\n");
      return NULL;
    }
    if(dest -> fcb . is_dir == 0){   //we got a first file Block with a FCB
      if(strncmp(filename, dest -> fcb.name, strlen(filename)) == 0){   //Same name file exits
        FileHandle* fh = (FileHandle*) malloc(sizeof(FileHandle));
        fh -> sfs = dir -> sfs;
        fh -> fcb = dest;
        fh -> directory = dir -> dcb;
        fh -> current_block = &dest -> header;
        fh -> pos_in_file = 0;
        return fh;
      }
    }
  }
  //No file with given name was found
  return NULL;
}

// LC
// closes an opened file by destroing the relative FileHandle
// return 0 on success, 1 if anything happens

//Is there a way to avoid double close on same fh?
int SimpleFS_close(FileHandle* fh){
  if(fh == NULL){
    return 1;         //closing a non-opened file
  }
  free(fh);
  return 0;
}


// LC
// writes in the file, from cursor position for size bytes, the data in data
// if necessary, allocates new blocks
// returns the number of bytes written or -1 if anything goes wrong

int SimpleFS_write(FileHandle* f, void* data, int size){
  int cursorStart = f -> pos_in_file;             //start index for writing
  int i;                                          //Master Loop Variable
  int res;                                        //Error management

  if(data == NULL || size <= 0) return 0;         //Self-explanatory
  char* dataToWrite = (char*) data;

  // by looking at the cursor, we know in which block we are going to start writing
  FirstFileBlock* ffb = f -> fcb;
  FileBlock* dest = (FileBlock*) ffb;
  FileBlock* previous = NULL;
  if(cursorStart < BLOCK_SIZE-sizeof(FileControlBlock) - sizeof(BlockHeader)){    //start writing in FirstFileBlock
    int h;
    for(h = 0; h < size; h++){      //start writing;
      if(cursorStart+h<(BLOCK_SIZE-sizeof(FileControlBlock) - sizeof(BlockHeader))){    //can safely write in data
        ffb->data[cursorStart+h] = dataToWrite[h];
      }
      else break;
    }
    if(h < size){   //there's still something to write
    //Creates new FileBlock, writes to disk and call write for the remaining data
        int nextFreeBlock = DiskDriver_getFreeBlock(f -> sfs -> disk, ffb -> header.next_block);
        if(nextFreeBlock == -1){  //No space left on disk
          return -1;
        }
        FileBlock* to_write = (FileBlock*) malloc(sizeof(FileBlock));
        to_write -> header.next_block = 0;
        to_write -> header.previous_block = ffb -> fcb.block_in_disk;
        to_write -> header.block_in_file = 1;   //next block after firstFileBlock
        res = DiskDriver_writeBlock(f -> sfs -> disk, to_write, nextFreeBlock);
        if(res != 0){     //something occcured while writing to disk
          return -1;
        }
        //updating values
        f -> pos_in_file+=h;
        ffb -> header.next_block = nextFreeBlock;
        f -> fcb -> fcb . size_in_bytes+=h;
        f -> fcb -> fcb.size_in_blocks += 1;
        f -> directory -> fcb . size_in_bytes += h;
        f -> directory -> fcb . size_in_blocks += 1;
        int recursiveWrite = SimpleFS_write(f, dataToWrite+h, size-h);   //keep writing on file
        return h+recursiveWrite;              //should be equal to size
    }
    else {      //everything done
        f -> pos_in_file += h;
        f -> fcb -> fcb . size_in_bytes+=h;
        f -> directory -> fcb . size_in_bytes += h;
        return h;         //should be equal to size

    }
  }
  else {                  //start writing in a FileBlock
      res = DiskDriver_readBlock(f -> sfs -> disk, (void**)&(dest), f -> fcb -> header.next_block);   //jumps firstFileBlock which is full
      cursorStart-=BLOCK_SIZE-sizeof(FileControlBlock) - sizeof(BlockHeader);
      int num_jumps = cursorStart % (BLOCK_SIZE-sizeof(BlockHeader));         //computes how many file blocks must be run over
      for(i = 0; i < num_jumps; i++){
        previous = dest;
        res = DiskDriver_readBlock(f -> sfs -> disk, (void**)&(dest), dest -> header.next_block);
        if(res != 1){   //Destination block is not written on disk, an error occured
          //printf("Block was not written\n");
          return -1;
        }
      }
      //here dest contains the destination block
      int j;
      int relativeCursor = cursorStart - num_jumps*(BLOCK_SIZE-sizeof(BlockHeader));    //relative position of cursor in the dest file block
      for(j = 0; j < size; j++){      //start writing;
        if(relativeCursor+j<(BLOCK_SIZE-sizeof(BlockHeader))){    //can safely write in data
          dest->data[relativeCursor+j] = dataToWrite[j];
        }
        else break;
      }
      if(j == 0){   //relative cursor points at end of block, a new block must be created and data written in it
        int nextFreeBlock = DiskDriver_getFreeBlock(f -> sfs -> disk, f -> fcb -> fcb . block_in_disk);
        if(nextFreeBlock == -1){  //No space left on disk
          //printf("No space left on disk\n");
          return -1;
        }

        FileBlock* to_write = (FileBlock*) malloc(sizeof(FileBlock));
        to_write -> header.next_block = 0;
        if(previous != NULL)
          to_write -> header.previous_block = previous -> header.next_block;
        else    //if previous == NULL, we are creating a new block right after the FirstFileBlock
          to_write -> header.previous_block = ffb -> fcb . block_in_disk;

        to_write -> header.block_in_file = dest -> header.block_in_file +1;
        res = DiskDriver_writeBlock(f -> sfs -> disk, to_write, nextFreeBlock);
        if(res != 0){     //something occcured while writing to disk
          //printf("Error while writing new block to disk\n");
          return -1;
        }
        //updating values
        dest -> header.next_block = nextFreeBlock;
        f -> pos_in_file+=j;
        f -> fcb -> fcb . size_in_bytes+=j;
        f -> fcb -> fcb.size_in_blocks += 1;
        f -> directory -> fcb . size_in_bytes += j;
        f -> directory -> fcb . size_in_blocks += 1;
        f -> pos_in_file += 1;            //forgot to add this, bad shit happened
        int recursiveWrite = SimpleFS_write(f, dataToWrite, size);   //keep writing on file
        if(recursiveWrite == -1){
          //printf("Something went wrong with recursive write\n");
          return -1;
        }
        return recursiveWrite;     //should be exactly size

      }

      else if(j < size){ //there's still data to write
      //Creates new FileBlock, writes to disk and call write for the remaining data
          int nextFreeBlock = DiskDriver_getFreeBlock(f -> sfs -> disk, previous -> header.next_block);
          if(nextFreeBlock == -1){  //No space left on disk
            //printf("No space left on disk\n");
            return -1;
          }
          FileBlock* to_write = (FileBlock*) malloc(sizeof(FileBlock));
          to_write -> header.next_block = 0;
          to_write -> header.previous_block = previous -> header.next_block;
          to_write -> header.block_in_file = dest -> header.block_in_file +1;
          res = DiskDriver_writeBlock(f -> sfs -> disk, to_write, nextFreeBlock);
          if(res != 0){     //something occcured while writing to disk
            //printf("Error while writing new block to disk\n");
            return -1;
          }
          dest -> header.next_block = nextFreeBlock;
          //updating values
          f -> pos_in_file+=j;
          f -> fcb -> fcb . size_in_bytes+=j;
          f -> fcb -> fcb.size_in_blocks += 1;
          f -> directory -> fcb . size_in_bytes += j;
          f -> directory -> fcb . size_in_blocks += 1;
          int recursiveWrite = SimpleFS_write(f, dataToWrite+j, size-j);   //keep writing on file
          if(recursiveWrite == -1){
            //printf("Something went wrong with recursive write\n");
            return -1;
          }
          return j+recursiveWrite;     //should be exactly size
      }
      //everything clear
      f -> pos_in_file += j;
      f -> fcb -> fcb.size_in_bytes+=j;
      f -> directory -> fcb . size_in_bytes += j;
      return j;   //should be equal to size, number of bytes written
  }
}
  // LC
  // reads in the preallocated data array, from current position of cursor,
  // data stored in the file provided
  // returns the number of bytes read or -1 if anything goes wrong

  //Might need some additional testing about error management
  int SimpleFS_read(FileHandle* file, void* data, int size){
    int absoluteCursor = file -> pos_in_file;
    int i;
    char* readInto = (char*) data;
    if(absoluteCursor < BLOCK_SIZE-sizeof(FileControlBlock) - sizeof(BlockHeader)){   //will start reading from FirstFileBlock
      for(i = 0; i < size; i++){
        if(absoluteCursor+i < BLOCK_SIZE-sizeof(FileControlBlock) - sizeof(BlockHeader)){   //Can safely read in block
          readInto[i] = file -> fcb -> data[absoluteCursor+i];
          file -> pos_in_file += 1;
        }
        else break;
      }
      if(i < size){       //there's still something to read
      int recursiveRead = SimpleFS_read(file, readInto+i, size-i);
      if(recursiveRead == -1) return -1;
      return i+recursiveRead;     //Should be equal to size
      }
      else {  //everything's done
      return i;
      }
    }

    else{     //Will start reading from a FileBlock

      //First jump across as many block as needed
      int relativeCursor = absoluteCursor - BLOCK_SIZE-sizeof(FileControlBlock) - sizeof(BlockHeader);
      int num_jumps = relativeCursor % (BLOCK_SIZE-sizeof(BlockHeader));
      relativeCursor -= num_jumps * (BLOCK_SIZE-sizeof(BlockHeader));
      FileBlock* dest = (FileBlock*) file -> fcb;
      int j, res;
      for(j = 0; j < num_jumps; j++){
        res = DiskDriver_readBlock(file -> sfs -> disk, (void**)&(dest), dest -> header.next_block);
        if(res != 1){
          //printf("Block was not written on disk\n");
          return -1;
        }
      }
      int h;
      for(h = 0; h < size; h++){
        if(relativeCursor+h < (BLOCK_SIZE-sizeof(BlockHeader))){    //can safely read data
          readInto[h] = dest -> data[relativeCursor+h];
          file -> pos_in_file += 1;
        }
      }

      if(h < size){ //There's still something to read
        int recursiveRead = SimpleFS_read(file, readInto+h, size-h);
        if(recursiveRead == -1) return -1;
        return h+recursiveRead;   //should be equal to size
      }

      else{       //everything's done
        return h;     //should be equal to size
      }
    }
  }

  // LC
  // changes cursor position from actual to pos
  // returns pos on success, -1 if anything goes wrong (file too short, ecc ecc)
  int SimpleFS_seek(FileHandle* file, int pos){
    if(pos < 0) return -1; //Can't get beyond 0
    int max = (BLOCK_SIZE-sizeof(FileControlBlock) - sizeof(BlockHeader)); //Dimension of FirstFileBlock data array
    max += (file -> fcb -> fcb . size_in_blocks -1) * (BLOCK_SIZE-sizeof(BlockHeader));  //Dimension of FileBlock data array
    if(pos > max) return -1; //File too short
    file -> pos_in_file = pos;     //new cursos position
    return pos;
  }

  // LC
  // makes dir point to given directory inside current directory
  // if dirname is .. , dir will go one level up
  // returns 0 on success, 1 if anything happens
  int SimpleFS_changeDir(DirectoryHandle* dir, char* dirname){
    if(dir == NULL || dirname == NULL) return 0;          //pretty self-explanatory
    else if(strncmp(dirname, "..", strlen("..")) == 0 && dir -> directory == NULL) {//We are at top level, cannot go anymore up
        return 1;
      }
    else if(strncmp(dirname, "..", strlen("..")) == 0){   //Let's go up one level
      dir -> dcb = dir -> directory;
      int res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(dir->directory), dir -> directory ->fcb . parent_directory);   //Retrieves from disk parent directory
      if(res!=1){
        //block was not written on disk, something happened
        return 1;
      }
      dir -> current_block = &dir -> dcb -> header;
      dir -> pos_in_dir = 0;
      dir -> pos_in_block = 0;
      return 0;
    }
    else{
      //We have to look for a directory with the given name in dir, return 1 if
      //it does not exists, else update dir and return 0

      int i, res, max = dir -> dcb -> num_entries;
      FirstDirectoryBlock* dest = (FirstDirectoryBlock*) dir->dcb;
      for(i = 0; i < max; i++){
        if(i < ((BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int))/sizeof(int) )){ //Can safely read inside the FirstDirectoryBlock
          int index = dir -> dcb ->file_blocks[i];
          if(index == FREE_BLOCK){  //skips previously deleted blocks
            max++;
            continue;
          }
          res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(dest), index);
          if(res != 1){
            //block was not written on disk, something occured
            return 1;
          }
          if(dest -> fcb . is_dir == 1){  //We have a first directory block
            if(strncmp(dirname, dest->fcb . name, strlen(dirname)) == 0){   //found the directory
              dir -> directory = dir -> dcb;
              dir -> dcb = dest;
              dir -> current_block = &dir->dcb->header;
              dir -> pos_in_dir = 0;
              dir -> pos_in_block = 0;
              return 0;
            }
          }
        }
        else break;
      }
      if(i < max){  //There are still directory blocks to check
        DirectoryBlock* src = (DirectoryBlock*) dest;
        int offset = (BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int))/sizeof(int);
        int size_dir = (BLOCK_SIZE-sizeof(BlockHeader))/sizeof(int);
        int num_jumps = 0;
        res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(src), dest -> header . next_block);
        if(res != 1){
          //Block was not written on disk, an error occured
          return 1;
        }
        for(; i < max; i++){      //But i still haven't found what i'm looking for
          if(i - offset - num_jumps*size_dir< ((BLOCK_SIZE-sizeof(BlockHeader))/sizeof(int))){//Can safely read in directory block
            int index = src ->file_blocks[i - offset - num_jumps*size_dir];
            if(index == FREE_BLOCK){
              continue;
              max++;
            }
            res = DiskDriver_readBlock(dir-> sfs -> disk, (void**)&(dest), index);
            if(res != 1){
              //Block was not written on disk, an error occured
              return 1;
            }
            if(dest -> fcb . is_dir == 1){  //We have a first directory block
              if(strncmp(dirname, dest->fcb . name, strlen(dirname)) == 0){   //found the directory
                dir -> directory = dir -> dcb;
                dir -> dcb = dest;
                dir -> current_block = &dir->dcb->header;
                dir -> pos_in_dir = 0;
                dir -> pos_in_block = 0;
                return 0;
              }
            }
          }
          else{ //proceed to next directory block
            res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(src), src -> header.next_block);
            if(res != 1){
              //Block was not written on disk, an error occured
              return 1;
            }
            num_jumps++;
          }
        }
        return 1;     //never found what i'm looking for
      }
    return 1;         //same old story
  }
}

  // LC
  // creates a new directory in the current one, linked by dir -> dcb -> header -> next_block
  // returns 0 on success, 1 if anything happens
  int SimpleFS_mkDir(DirectoryHandle* dir, char* dirname){
    if(dir == NULL || dirname == NULL) return -1;      //pretty self explanatory
    if(strncmp(dirname, "/", strlen("/")) == 0) {     //Cannot create a dir named /
      return 1;
    }
    int nextFreeBlock = DiskDriver_getFreeBlock(dir -> sfs -> disk, dir -> dcb -> fcb . block_in_disk);
    if(nextFreeBlock == -1){
      //printf("Not enough space on disk\n");
      return 1;    //No space left on disk
    }
    //Creating a new FirstDirectoryBlock
    FirstDirectoryBlock* toWrite = (FirstDirectoryBlock*) malloc(sizeof(FirstDirectoryBlock));
    //updating its fcb
    toWrite -> fcb . parent_directory = dir -> dcb -> fcb . block_in_disk;
    toWrite -> fcb . block_in_disk = nextFreeBlock;
    strncpy(toWrite-> fcb . name, dirname, strlen(dirname));
    toWrite -> fcb . size_in_bytes = 0;
    toWrite -> fcb . size_in_blocks = 1;
    toWrite -> fcb . is_dir = 1;     //yes it is

    //updating its header and linking new block to previous ones

    toWrite -> header . previous_block = 0;
    toWrite -> header . next_block = 0;
    toWrite -> header . block_in_file = 0;    //contains an fcb

    //updating other values
    toWrite -> num_entries = 0;

    //Writing block on disk
    int res = DiskDriver_writeBlock(dir -> sfs -> disk, toWrite, nextFreeBlock);
    if(res != 0){
      return 1;  //Something went wrong with writing block
    }

    //New dir block created, linking with previous one
    if(dir -> dcb -> num_entries < ((BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int))/sizeof(int) )){
      //write new block in FirstDirectoryBlock
      int h;
      for(h = 0; h < dir->dcb->num_entries; h++){
        if(dir -> dcb -> file_blocks[h] == FREE_BLOCK){
          //previously deleted block found
          dir -> dcb -> file_blocks[h] = nextFreeBlock;
          dir -> dcb -> num_entries++;
          dir -> dcb -> fcb.size_in_blocks++;
          return 0;
        }
      }
        dir -> dcb -> file_blocks[dir -> dcb -> num_entries] = nextFreeBlock;
        dir -> dcb -> num_entries++;
        dir -> dcb -> fcb . size_in_blocks++;
        return 0;
    }
    else{
      DirectoryBlock* dest = (DirectoryBlock*) dir -> dcb;
      if(dest -> header . next_block == 0){
        //check for space
        int posForDir = DiskDriver_getFreeBlock(dir -> sfs -> disk, dest -> header.next_block);
        if(posForDir == -1){
          //no space left on disk for new DirectoryBlock
          return 1;
        }
        //create a new DirectoryBlock
        DirectoryBlock* newDir = (DirectoryBlock*) malloc(sizeof(DirectoryBlock));
        newDir -> header . next_block = 0;
        newDir -> header . previous_block = dir -> dcb -> fcb . block_in_disk;
        newDir -> header . block_in_file = 1;     //second block of the chain
        newDir -> file_blocks[0] = nextFreeBlock;
        //Writing new block on disk
        res = DiskDriver_writeBlock(dir -> sfs -> disk, newDir, posForDir);
        if(res != 0){
          return 1;  //Something went wrong with writing block
        }
        //linking new block and updating contents
        dest -> header . next_block = posForDir;
        dir -> dcb -> num_entries++;
        dir -> dcb -> fcb . size_in_blocks ++;

        return 0;       //everything done
      }
      else {
        int num = 0;
        int previous_index = dir -> dcb -> fcb . block_in_disk;
        int numBlock = 1;
        while(dest -> header . next_block != 0){      //skips through all directory blocks, looking for a free space in that block
          previous_index = dest -> header.next_block;
          res = DiskDriver_readBlock(dir -> sfs -> disk, (void**) &(dest), dest -> header . next_block);
          if(res != 1){
            return 1;  //Something went wrong with reading block
          }
          int j;
          for(j = 0; j < (BLOCK_SIZE-sizeof(BlockHeader))/sizeof(int); j++){
            if(dest -> file_blocks[j] == FREE_BLOCK){
              dest -> file_blocks[j] = nextFreeBlock;
              dir -> dcb -> num_entries++;
              dir -> dcb -> fcb.size_in_blocks++;
              return 0;
            }
          }
          num++;
          numBlock++;
        }
        //here dest is the last directory block of the chain
        int offset = (BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int))/sizeof(int);
        int relativeIndex = dir -> dcb -> num_entries - offset - num*((BLOCK_SIZE-sizeof(BlockHeader))/sizeof(int));
        if(relativeIndex < (BLOCK_SIZE-sizeof(BlockHeader))/sizeof(int)){ //Can write in dest
          int j;
          for(j = 0; j < relativeIndex; j++){
            if(dest -> file_blocks[j] == FREE_BLOCK){
              dest -> file_blocks[relativeIndex] = nextFreeBlock;
              dir -> dcb -> num_entries++;
              dir -> dcb -> fcb.size_in_blocks++;
              return 0;
            }
          }
          dest -> file_blocks[relativeIndex] = nextFreeBlock;
          //updating contents
          dir -> dcb -> num_entries++;
          dir -> dcb -> fcb . size_in_blocks ++;

          return 0; //Everything's done
        }
        else {
          //A new directoryBlock must be created and linked
          int posForDir = DiskDriver_getFreeBlock(dir -> sfs -> disk, dest -> header.next_block);
          if(posForDir == -1){
            //no space left on disk for new DirectoryBlock
            return 1;
          }
          //create a new DirectoryBlock
          DirectoryBlock* newDir = (DirectoryBlock*) malloc(sizeof(DirectoryBlock));
          newDir -> header . next_block = 0;
          newDir -> header . previous_block = previous_index;
          newDir -> header . block_in_file = numBlock;     //i-th block of the chain
          newDir -> file_blocks[0] = nextFreeBlock;
          //Writing new block on disk
          res = DiskDriver_writeBlock(dir -> sfs -> disk, newDir, posForDir);
          if(res != 0){
            return 1;  //Something went wrong with writing block
          }
          //linking new block and updating contents
          dest -> header . next_block = posForDir;
          dir -> dcb -> num_entries++;
          dir -> dcb -> fcb . size_in_blocks ++;

          return 0;       //everything done
        }
      }
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

  printf("\n");

  return;
}

// BEGIN EXPERIMENTAL
// LC
// Removes the given file and all his blocks from his dir
// Returns 0 on success, 1 if anything happens
int SimpleFS_rmFile(DirectoryHandle* dir, char* filename){
  if(dir == NULL || filename == NULL) return 0;  //Nothing to do here
  int res;
  FirstDirectoryBlock* container = dir -> dcb;
  //Checks if file is in provided dir
  char** names = (char**) malloc(sizeof(char*)*dir -> dcb -> num_entries);
  res = SimpleFS_readDir(names, dir, 0);
  if(res != 0) return 1;    //something happened with readDir
  int j, found = 0;
  for(j = 0; j < dir -> dcb -> num_entries; j++){
    if(names[j] != NULL && strncmp(names[j], filename, strlen(filename)) == 0){
      found = 1;
      break;
    }
  }
  if(!found) return 0; //404 file not found
  FirstFileBlock* file;

  //int indexOfFile = file -> fcb -> fcb . block_in_disk;
  FileBlock* auxFile;
  //Will now look for our file in the directory and replace it with FREE BLOCK
  int i, num_entries =  container -> num_entries;
  for(i = 0; i < num_entries; i++){
    if(i < (BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int))/sizeof(int)){//Can safely read in FirstDirectoryBlock
      if(dir -> dcb -> file_blocks[i] == FREE_BLOCK){
        num_entries++;
        continue;
      }
      res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(file), dir -> dcb -> file_blocks[i]);
      if(res != 1) return 0; //error while reading block
      if(file -> fcb . is_dir == 0 && strncmp(filename, file -> fcb . name, strlen(filename)) == 0){    //File found, removing it and updating directory params
        container->file_blocks[i] = FREE_BLOCK;
        //Updating values
        container-> num_entries --;
        container-> fcb . size_in_bytes -= file -> fcb .size_in_bytes;
        //Freeing file from disk
        int nextBlock = file -> header . next_block;
        res = DiskDriver_freeBlock(dir -> sfs -> disk, file -> fcb . block_in_disk);
        if(res != 0){
          return 1;
        }
        while(nextBlock!=0){    //let's free all file blocks;
          res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(auxFile), nextBlock);
          if(res!=1){
            //Error occured with readBlock
            return 1;
          }
          int prev = nextBlock;
          nextBlock = auxFile -> header . next_block;
          res = DiskDriver_freeBlock(dir -> sfs -> disk, prev);
          if(res!= 0){
            //Error with freeBlock
            return 1;
          }
        }
        return 0;
      }
    }
    else break;   //We have to skip to next directory block
  }
  //if here, file was not found in FirstDirectoryBlock
  //We need to look through the others blocks
  DirectoryBlock* aux;
  res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(aux), container -> header . next_block);
  if(res != 1){   //Error on reading block
    return 1;
  }
  DirectoryBlock* previous = (DirectoryBlock*) container;
  int num = 0;
  int offset = (BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int))/sizeof(int);
  int blocksInDirBlock = (BLOCK_SIZE-sizeof(BlockHeader))/sizeof(int);
  for(; i < num_entries; i++){
    if(i - offset - num * blocksInDirBlock < (BLOCK_SIZE-sizeof(BlockHeader))/sizeof(int)){   //can safely read from current dir block
      if(aux->file_blocks[i] == FREE_BLOCK){
        num_entries++;
        continue;
      }
      res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(file), aux -> file_blocks[i]);
      if(res != 1) return 1; //Something went off with read block
      if(file -> fcb . is_dir == 0 && strncmp(filename, file -> fcb. name, strlen(filename)) == 0){   //file found
        aux -> file_blocks[i-offset] = FREE_BLOCK;
        container-> num_entries --;
        container-> fcb . size_in_bytes -= file -> fcb .size_in_bytes;

        // we want to remove a dirBlock if it's now empty (so it's the last one in the chain)
        if(aux -> header.next_block == 0 && (container->num_entries - offset) % blocksInDirBlock == 0){ //aux is now an empty dir block, I'll remove it
          res = DiskDriver_freeBlock(dir -> sfs -> disk, previous -> header . next_block);
          if(res != 0){ //Error occured while deleting dir block
            return 1;
          }
          previous -> header . next_block = 0;
          container -> fcb . size_in_blocks --;
        }
        //Freeing file from disk and fileHandle
        int nextBlock = file -> header . next_block;
        res = DiskDriver_freeBlock(dir -> sfs -> disk, file -> fcb . block_in_disk);
        if(res != 0){
          return 1;
        }
        while(nextBlock!=0){    //let's free all file blocks;
          res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(auxFile), nextBlock);
          if(res!=1){
            //Error occured with readBlock
            return 1;
          }
          int prev = nextBlock;
          nextBlock = auxFile -> header . next_block;
          res = DiskDriver_freeBlock(dir -> sfs -> disk, prev);
          if(res!= 0){
            //Error with freeBlock
            return 1;
          }
        }
        return 0;
      }
    }
    else{//load in next dir block
      if(aux -> header .next_block == 0){//Reached end of dir, no file found, error
        return 1;
      }
      previous = aux;
      res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(aux), aux -> header . next_block);
      if(res != 1){     //Error while reading dir from disk
        return 1;
      }
      num++;
    }
  }
  //End of dir, no file found, error
  return 1;
}

// LC
// Removes the given dir and all his contents (recursively) from fs
// returns 0 on success, 1 if anything gows wrong
// Side-effect on dir, it will point to parent directory

int SimpleFS_rmDir(DirectoryHandle* dir){
  if(dir == NULL || dir -> directory == NULL) return 0; //Nothing to do here, can't remove root or null dir

  int res, i, num_entries = dir -> dcb -> num_entries;
  char** filenames = (char**) malloc(sizeof(char*) * num_entries);
  res = SimpleFS_readDir(filenames, dir, 0);    //we'll read all files inside this dir
  for(i = 0; i < num_entries; i++){
    if(filenames[i] != NULL){
      res = SimpleFS_rmFile(dir, filenames[i]);
      if(res != 0){ //Something happened while removing file
        return 1;
      }
    }
  }
    free(filenames);
    //All file eliminated, eliminating now dirs
    //Now readDir on dir returns only FirstDirectoryBlock

    //Removing dirs in dir
    char** dirnames = (char**) malloc(sizeof(char*) * dir -> dcb -> num_entries);
    for(i = 0; i < dir -> dcb -> num_entries; i++){
      if(dirnames[i] != NULL){
        res = SimpleFS_changeDir(dir, dirnames[i]);
        if(res != 0) return 1;    //Something wrong with changeDir
        res = SimpleFS_rmDir(dir);
        if(res != 0) return 1;    //something wrong with rmDir
        res = SimpleFS_changeDir(dir, "..");
        if(res != 0) return 1;    //something wrong with changeDir
      }
    }
    free(dirnames);

    //Eliminating dir
    int indexOfDir = dir -> dcb -> fcb . block_in_disk;
    res = SimpleFS_changeDir(dir, "..");
    if(res != 0) return 1;
    int max = dir -> dcb -> num_entries;
    for(i = 0; i < max; i++){
      if(i < (BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int))/sizeof(int)){
        int index = dir -> dcb -> file_blocks[i];
        if(index == FREE_BLOCK){
          max++;
          continue;
        }
        if(index == indexOfDir){
          dir -> dcb -> file_blocks[i] = FREE_BLOCK;
          dir -> dcb -> num_entries--;
          res = DiskDriver_freeBlock(dir -> sfs -> disk, indexOfDir);
          if(res != 0) return 1;
          return 0;
        }
      }
      else break;
  }
    //if here, dir not found, must look in another directory block
    DirectoryBlock dirBlock;
    DirectoryBlock* aux = &dirBlock;
    int nextDirBlock = dir -> dcb -> header . next_block;
    int num_jumps = 0, offset = (BLOCK_SIZE-sizeof(BlockHeader)-sizeof(FileControlBlock)-sizeof(int))/sizeof(int);
    while(nextDirBlock != 0){
      res = DiskDriver_readBlock(dir -> sfs -> disk, (void**)&(aux), nextDirBlock);
      if(res != 1) return 1;
    }
    for(; i < max; i++){
      if(i -offset - num_jumps*(BLOCK_SIZE-sizeof(BlockHeader))/sizeof(int) < (BLOCK_SIZE-sizeof(BlockHeader))/sizeof(int)){
        if(aux -> file_blocks[i] == FREE_BLOCK){
          max++;
          continue;
        }
        else if(aux -> file_blocks[i] == indexOfDir){
          aux -> file_blocks[i] = FREE_BLOCK;
          dir -> dcb -> num_entries--;
          res = DiskDriver_freeBlock(dir -> sfs -> disk, indexOfDir);
          if(res != 0) return 1;
          return 0;
        }
      }
      else {
        num_jumps++;
        nextDirBlock = aux -> header . next_block;
        break;
      }
    }
    return 0;

}
// LC
// prints all files and dir in provided directory using SimpleFS_readDir
// returns 0 on success, 1 if anything happens
int SimpleFS_ls(DirectoryHandle* dh){
  if(dh == NULL) return 0; //nothing to do here
  char** names = (char**) malloc(sizeof(char*)* dh -> dcb -> num_entries);
  int res = SimpleFS_readDir(names, dh, 1);
  if(res != 0) return 1;
  int i;
  for(i = 0; i < dh -> dcb -> num_entries; i++){
    if(names[i] != NULL) printf("%s  ", names[i]);
  }
  printf("\n");
  free(names);
  return 0;
}

// LC
// frees all memory used and removes fileSystem and Disk
// returns 0 on success, 1 if anything goes wrong
// can only be used on root
// final destination
int SimpleFS_rmslash(DirectoryHandle* root){ //finally removing root, fs and disk
    int res = DiskDriver_delete(root -> sfs -> disk);
    if(res != 0) return 1;
    free(root -> current_block);
    free(root -> sfs);
    free(root);     //Rest in peace
    return 0;
}

// LC
// prints info about blockHeader values, checks if anything is correct
void print_info_block_header(BlockHeader* bh){
  printf("Header: prev %d\tnext %d\tblock_in_file %d\n", bh -> previous_block, bh -> next_block, bh -> block_in_file);
  printf("\n");
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
  printf("\n");
  return;
}
