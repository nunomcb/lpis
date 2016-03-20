#include <stdlib.h>
#include "stack.h"

typedef void (*DestroyFunc)(void*);

struct StackNode_s {
    void* value;
    StackNode prev;
};

struct Stack_s {
    int size;
    DestroyFunc destroy_value;
    StackNode top;
};

/**
 * STACK NODE
 */

static StackNode stack_node_new(void* value, StackNode prev) {
    StackNode node = (StackNode)malloc(sizeof(struct StackNode_s));

    node->value = value;
    node->prev = prev;

    return node;
}

static void stack_node_destroy(StackNode node, DestroyFunc destroy_value) {

    if (destroy_value)
        destroy_value(node->value);

    free(node);
}

void* stack_node_get_value(StackNode node) {
    return node->value;
}

StackNode stack_node_get_prev(StackNode node) {
    return node->prev;
}

int stack_node_has_prev(StackNode node) {
    return node->prev != NULL;
}

/**
 * STACK
 */

Stack stack_new(DestroyFunc destroy_value) {
    Stack s = (Stack)malloc(sizeof(struct Stack_s));

    s->size = 0;
    s->top = NULL;
    s->destroy_value = destroy_value;

    return s;
}

void stack_destroy(Stack s) {
    StackNode curr, prev;

    curr = s->top;

    while (curr) {
        prev = curr->prev;

        stack_node_destroy(curr, s->destroy_value);

        curr = prev;
    }

    free(s);
}

void stack_push(Stack s, void* value) {
    s->top = stack_node_new(value, s->top);
    s->size++;
}

void* stack_pop(Stack s) {
    void* value;
    StackNode prev;

    if (stack_is_empty(s))
        return NULL;

    value = s->top->value;
    prev = s->top->prev;

    stack_node_destroy(s->top, NULL);
    s->top = prev;
    s->size--;

    return value;
}

void* stack_get_top(Stack s) {

    if (stack_is_empty(s))
        return NULL;

    return s->top->value;
}

StackNode stack_get_top_node(Stack s) {
    return s->top;
}

int stack_get_size(Stack s) {
    return s->size;
}

int stack_is_empty(Stack s) {
    return s->top == NULL;
}
