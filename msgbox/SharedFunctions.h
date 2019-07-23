#pragma once
#include <stdio.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include<errno.h>
#include <signal.h>

void* SharedCreate(char* name,int size,int flag); //crea la shm e la mmappa per tutti i processi che ne hanno bisogno,viene chiamata UNA volta AF MG FF
int SharedWrite(char* text, void* memory); // scrive nella memoria condivisa il messaggio, restituendo per altro la lunghezza del messaggio, in modo che possa essere aggiunta al puntatore nel processo AF MG FF
int SharedRead(void*memory); // legge il contenuto della memoria AF MG FF
