#include "linked_list.h"
#include <assert.h>

void list_init(listHead* head) {
  head->first=0;
  head->last=0;
  head->size=0;
}

listItem* list_find(listHead* head, listItem* item) {
  // linear scanning of list
  listItem* aux=head->first;
  while(aux){
    if (aux==item)
      return item;
    aux=aux->next;
  }
  return 0;
}

listItem* list_insert(listHead* head, listItem* prev, listItem* item) {
  if (item->next || item->prev)
    return 0;

#ifdef _list_DEBUG_
  // we check that the element is not in the list
  listItem* instance=list_find(head, item);
  assert(!instance);

  // we check that the previous is inthe list

  if (prev) {
    listItem* prev_instance=list_find(head, prev);
    assert(prev_instance);
  }
  // we check that the previous is inthe list
#endif

  listItem* next= prev ? prev->next : head->first;
  if (prev) {
    item->prev=prev;
    prev->next=item;
  }
  if (next) {
    item->next=next;
    next->prev=item;
  }
  if (!prev)
    head->first=item;
  if(!next)
    head->last=item;
  ++head->size;
  return item;
}

listItem* list_detach(listHead* head, listItem* item) {

#ifdef _list_DEBUG_
  // we check that the element is in the list
  listItem* instance=list_find(head, item);
  assert(instance);
#endif

  listItem* prev=item->prev;
  listItem* next=item->next;
  if (prev){
    prev->next=next;
  }
  if(next){
    next->prev=prev;
  }
  if (item==head->first)
    head->first=next;
  if (item==head->last)
    head->last=prev;
  head->size--;
  item->next=item->prev=0;
  return item;
}
