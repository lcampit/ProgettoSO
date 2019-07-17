#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

void* SharedCreate(char* name); //crea la shm e la mmappa per tutti i processi che ne hanno bisogno,viene chiamata UNA volta AF MG FF
int SharedWrite(char* text, void* memory); // scrive nella memoria condivisa il messaggio, restituendo per altro la lunghezza del messaggio, in modo che possa essere aggiunta al puntatore nel processo AF MG FF
void SharedRead(void*memory); // legge il contenuto della memoria AF MG FF
