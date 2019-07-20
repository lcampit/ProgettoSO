#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void* SharedCreate(char* name, int size, int flag){ //crea la shm e la mmappa per tutti i processi che ne hanno bisogno,viene chiamata UNA volta
  int fd;
  if (flag == 0){
  fd = shm_open(name, O_RDWR|O_CREAT, 0666);
}
else{
  fd = shm_open(name, O_RDWR, 0666);
}
  if(fd < 0){
    printf("Cannot create shm\n");
    exit(-1);
  }
  int res = ftruncate(fd, size);
  if(res < 0){
    printf("Cannot truncate shm\n");
    exit(-1);
  }
  void* memory;
  if (flag == 0){
   memory = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);

 }
 else{
   memory = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
 }
 return memory;
}

int SharedWrite(char* text, void* memory){ //scrive nella shm il messaggio, restituendo la lunghezza dello stesso, in modo che possa essere aggiunta al puntatore nel processo
  return 0;
}

int SharedRead(void* memory){ //legge il contenuto della shm
  return 0;
}
