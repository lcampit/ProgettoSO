#pragma once

typedef struct listItem {
  struct listItem* prev;
  struct listItem* next;
} listItem;

typedef struct listHead {
  listItem* first;
  listItem* last;
  int size;
} listHead;

void list_init(listHead* head);   //Creates list
listItem* list_find(listHead* head, listItem* item); //Finds item in list
listItem* list_insert(listHead* head, listItem* previous, listItem* item); //inserts item in list after previous
listItem* list_detach(listHead* head, listItem* item);  //deletes item from list withouth freeing it
