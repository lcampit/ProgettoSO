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

void list_init(listHead* head);
listItem* list_find(listHead* head, listItem* item);
listItem* list_insert(listHead* head, listItem* previous, listItem* item);
listItem* list_detach(listHead* head, listItem* item);
