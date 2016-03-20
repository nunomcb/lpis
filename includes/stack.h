#ifndef STACK_H
#define STACK_H

typedef struct StackNode_s* StackNode;
typedef struct Stack_s* Stack;

/**
 * STACK
 */

Stack stack_new(void (*destroy_value)(void*));
void stack_destroy(Stack s);

void stack_push(Stack s, void* value);
void* stack_pop(Stack s);
void* stack_get_top(Stack s);

int stack_get_size(Stack s);
int stack_is_empty(Stack s);

StackNode stack_get_top_node(Stack s);

/**
 * STACKNODE
 */

void* stack_node_get_value(StackNode node);
StackNode stack_node_get_prev(StackNode node);
int stack_node_has_prev(StackNode node);

#endif
