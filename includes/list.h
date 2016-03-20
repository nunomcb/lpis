#ifndef LIST_H
#define LIST_H

typedef struct List_s* List;

List list_new(void (*destroy_value)(void*));
void list_destroy(List l);

void list_add(List l, void* value);
void* list_get(List l, int index);

int list_size(List l);

void* list_get_next(List l);

#endif
