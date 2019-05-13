#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "linked_list.h"

#define MAX_NUM_ITEMS 64

typedef struct IntlistItem{
  listItem list;
  int info;
} IntlistItem;

void Intlist_print(listHead* head){
  listItem* aux=head->first;
  printf("[");
  while(aux){
    IntlistItem* element = (IntlistItem*) aux;
    printf("%d ", element->info);
    aux=aux->next;
  }
  printf("]\n");
}

int main(int argc, char** argv) {
  listHead head;
  list_init(&head);
  for (int i=0; i<MAX_NUM_ITEMS; ++i){
    IntlistItem* new_element= (IntlistItem*)
      malloc(sizeof(IntlistItem));
    if (! new_element) {
      printf("out of memory\n");
      break;
    }
    new_element->list.prev=0;
    new_element->list.next=0;
    new_element->info=i;
    listItem* result=
      list_insert(&head, head.last, (listItem*) new_element);
    assert(result);
  }
  Intlist_print(&head);

  printf("removing odd elements");
  listItem* aux=head.first;
  int k=0;
  while(aux){
    listItem* item=aux;
    aux=aux->next;
    if (k%2){
      list_detach(&head, item);
      free(item);
    }
    ++k;
  }
  Intlist_print(&head);

  printf("removing from the head, half of the list");
  int size=head.size;
  k=0;
  while(head.first && k<size/2){
    listItem* item=list_detach(&head, head.first);
    assert(item);
    free(item);
    ++k;
  }
  Intlist_print(&head);

  printf("removing from the tail the rest of the list, until it has 1 element");
  while(head.first && head.size>1){
    listItem* item=list_detach(&head, head.last);
    assert(item);
    free(item);
  }

  Intlist_print(&head);

  printf("removing last element");
  listItem* item=list_detach(&head, head.last);
  assert(item);
  free(item);
  Intlist_print(&head);

}
