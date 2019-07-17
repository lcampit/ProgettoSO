#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>



void* SharedCreate(char* name){ //crea la shm e la mmappa per tutti i processi che ne hanno bisogno,viene chiamata UNA volta
  int fd=shm_open(name, O_RDWR|O_CREAT, 0666);
  if(fd<0){
    printf("Cannot create shm\n");
    exit(-1);
  }
  int res= ftruncate(fd, SIZE);
  if(res<0){
    printf("Cannot truncate shm\n");
    exit(-1);
  }
  void* memory= mmap(NULL,SIZE,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
   return memory;

}

int SharedWrite(char* text, void* memory){
  char* buffer= (char*) memory;
  sprintf(buffer, "%s\n",text);
  printf("Sending the message...\n");
  int len= strlen(text)+1;
  return len;
}

void SharedRead(void*memory){
  char* buffer= (char*) memory;
  printf("%s",buffer);
}
