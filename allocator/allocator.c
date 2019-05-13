#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "allocator.h"
#define MINBLOCKSIZE 4

buffer* buffer_init(int bytes){
	buffer res;
	BitMap_init(&res.bitmap, (int)(((bytes/MINBLOCKSIZE)*2) -1));
	res.memory = unsigned char[bytes];
	res.block_size = MINBLOCKSIZE;
	res.max = bytes;
	res.levels = buddylevel(res, MINBLOCKSIZE);
	return &res;
}

void* my_alloc(buffer* buff, int size){
	int level= buddylevel(buff,size);
	printf("%d\n", size);
	printf("level %d\n", level);
	int levello = level;
	int k = 0;
	int i=pow(2,level);
	int stop=pow(2,level+1);
	for(;i<stop;i++){
		if(BitMap_get(buff->check,i)==0){
			int j= i/2;
			while(level){
				//printf("%d\n",j);
				if(BitMap_get(buff->check,j)==1) break;
				BitMap_set(buff->check,j);
				level--;
				j=j/2;
			}
			set_children(buff,i,levello);
			int num = buff->max/(pow(2, levello));
			return (void*)&(buff->memory[num*k*buff->block_size]);
		}
		k++;
	}

}


void my_free(buffer* b, void* ptr, int size){
	int ind = ptr;
	int level = buddylevel(b, size);
	int num = pow(2, level);
	int start = &(b->memory[0]);
	int end = &(b->memory[b->max]);
	int dim = (start - end);
	int step = dim/num;
	int i = (ind - start)/dim;
	if((ind - start)%dim != 0) i++;
	int pos = num +i; //posizione nella bitmap
	BitMap_unset(b->check,pos);// block freed!
	//let's free the children!
	unset_children(b,pos,level);
	//let's free the parents!(if the buddy is free, of course!)
	unset_parents(b,pos,level);
	return;


}

int buddylevel(buffer* buff,int size){
     if(size>buff->max){
	fprintf(stderr,"not enough memory!\n");
	exit(1);
}
     int pow=0;
     int aux=buff->max;
     aux=aux/2;
	while (size<=aux){
		pow++;
		aux=aux/2;
    }
	return pow;

}

void set_children(buffer*buf,int i,int level){
	BitMap_set(buf->check,i);
	if(level==buf->levels+1) return;
	else{
		set_children(buf,2*i,level+1);
		set_children(buf,2*i+1,level+1);
	}
}

void unset_children(buffer*buf,int i,int level){
	BitMap_unset(buf->check,i);
	if(level==buf->levels+1) return;
	else{
		unset_children(buf,2*i,level+1);
		unset_children(buf,2*i+1,level+1);
	}
}

void unset_parents(buffer* buf, int i,int level){
	int idx=0;
	if(i%2==0) idx=1;
	int j= i/2;
	if(BitMap_get(buf->check,(j*2)+idx)==0){
		BitMap_unset(buf->check,j);
		if(level!=0){
			unset_parents(buf,j,level-1);
		}
	}
	return;
}
