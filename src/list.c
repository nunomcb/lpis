#include "list.h"
#include <stdlib.h>

typedef struct ListNode_s* ListNode;
typedef void (*DestroyFunc)(void*);

struct ListNode_s {
    void* value;
    ListNode next;
};

struct List_s {
    int size;
    DestroyFunc destroy_value;
    ListNode first;
    ListNode last;
    ListNode curr;
};

static ListNode list_node_new(void* value, ListNode next) {
    ListNode node = (ListNode)malloc(sizeof(struct ListNode_s));

    node->value = value;
    node->next = next;

    return node;
}

static void list_node_destroy(ListNode node, DestroyFunc destroy_value) {
    if (destroy_value)
        destroy_value(node->value);

    free(node);
}

List list_new(DestroyFunc destroy_value) {
    List list = (List)malloc(sizeof(struct List_s));

    list->size = 0;
    list->destroy_value = destroy_value;
    list->first = NULL;
    list->last = NULL;
    list->curr = NULL;

    return list;
}

void list_destroy(List l) {
    ListNode curr, next;

    curr = l->first;

    while (curr) {
        next = curr->next;

        list_node_destroy(curr, l->destroy_value);

        curr = next;
    }

    free(l);
}

void list_add(List l, void* value) {
    ListNode node = list_node_new(value, NULL);

    if (l->size > 0) {
        l->last->next = node;
        l->last = node;
    }
    else {
        l->first = l->last = node;  
    }

    l->size++;
}

void* list_get(List l, int index) {
    ListNode node;
    int i;

    node = l->first;
    i = 0;

    while (node && i < index) {
        node = node->next;
        i++;
    }

    if (node)
        return node->value;
    
    return NULL;
}

int list_size(List l) {
    return l->size;
}

void* list_get_next(List l) {
    if (!l->curr)
        l->curr = l->first;
    else
        l->curr = l->curr->next;

    if (!l->curr)
        return NULL;
    else
        return l->curr->value;
}
