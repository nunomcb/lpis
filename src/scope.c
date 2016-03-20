#include "scope.h"
#include "hash_table.h"
#include "stack.h"

#include <stdlib.h>

typedef struct ScopeCont_s* ScopeCont;

struct Scope_s {
    Stack stack;
};

struct ScopeCont_s {
    int vars_size;
    HashTable vars;
    HashTable functions;    
};

static void __var_destroy(void* var) {
    var_destroy((Variable)var);
}

static void __func_destroy(void* func) {
    func_destroy((Function)func);
}

static ScopeCont scope_cont_new() {
    ScopeCont scont = (ScopeCont)malloc(sizeof(struct ScopeCont_s));

    scont->vars_size = 0;
    scont->vars = htable_new(20, __var_destroy);
    scont->functions = htable_new(20, __func_destroy);

    return scont;
}

static void scope_cont_destroy(ScopeCont scont) {

    htable_destroy(scont->vars);
    htable_destroy(scont->functions);

    free(scont);
}

static void __scope_cont_destroy(void* scont) {
    scope_cont_destroy((ScopeCont) scont);
}

Scope scope_new() {
    Scope scope = (Scope)malloc(sizeof(struct Scope_s));

    scope->stack = stack_new(__scope_cont_destroy);

    return scope;
}

void scope_destroy(Scope scope) {
    
    stack_destroy(scope->stack);
    free(scope);
}

int scope_def_var(Scope scope, char* id, Variable var) {    
    ScopeCont scont = (ScopeCont)stack_get_top(scope->stack);

    if (htable_has_key(scont->vars, id))
        return 1;

    htable_put(scont->vars, id, var);

    scont->vars_size += var_get_size(var);

    return 0;
}

int scope_def_function(Scope scope, char* id, Function func) {
    ScopeCont scont = (ScopeCont)stack_get_top(scope->stack);

    if (htable_has_key(scont->functions, id))
        return 1;

    htable_put(scont->functions, id, func);

    return 0;
}

Variable scope_get_var(Scope scope, char* id, int* level) {
    StackNode node;
    ScopeCont scont;
    Variable var;
    int l;

    node = stack_get_top_node(scope->stack);

    l = scope_get_level(scope);

    while (node) {
        scont = (ScopeCont)stack_node_get_value(node);
        var = (Variable)htable_get(scont->vars, id);

        if (var) {
            *level = l;
            return var;
        }

        node = stack_node_get_prev(node);
        l--;
    }

    return NULL;
}

Function scope_get_function(Scope scope, char* id, int* level) {
    StackNode node = stack_get_top_node(scope->stack);
    ScopeCont scont;
    Function f;
    
    *level = scope_get_level(scope);

    while (node) {
        scont = (ScopeCont)stack_node_get_value(node);
        f = (Function)htable_get(scont->functions, id);

        if (f)
            return f;

        node = stack_node_get_prev(node);
        *level = *level - 1;
    }

    return NULL;
}

int scope_get_level(Scope scope) {
    return stack_get_size(scope->stack);
}

int scope_get_vars_size(Scope scope) {
    return ((ScopeCont)stack_get_top(scope->stack))->vars_size;
}

int scope_has_var(Scope scope, char* id) {
    int l;

    return scope_get_var(scope, id, &l) != NULL;
}

int scope_has_function(Scope scope, char* id) {
    int l;

    return scope_get_function(scope, id, &l) != NULL;
}

void scope_enter_subscope(Scope scope) {
    ScopeCont scont = scope_cont_new();

    stack_push(scope->stack, scont);
}

void scope_exit_subscope(Scope scope) {
    ScopeCont scont = stack_pop(scope->stack);

    scope_cont_destroy(scont);
}
