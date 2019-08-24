#pragma once
#include <semaphore.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

void* SharedCreate(char* name, int size, int flag); //crea la shm e la mmappa per tutti i processi che ne hanno bisogno, viene chiamata UNA volta AF MG FF
int SharedWrite(char* text, void* memory); //scrive nella shm il messaggio, restituendo la lunghezza dello stesso, in modo che possa essere aggiunta al puntatore nel processo AF MG FF
int SharedRead(void* memory); //legge il contenuto della shm AF MG FF
