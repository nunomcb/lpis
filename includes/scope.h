#ifndef SCOPE_H
#define SCOPE_H

#include "scope.h"
#include "function.h"
#include "variable.h"

typedef struct Scope_s* Scope;

Scope scope_new();
void scope_destroy(Scope scope);

int scope_def_var(Scope scope, char* id, Variable var);
int scope_def_function(Scope scope, char* id, Function func);

Variable scope_get_var(Scope scope, char* id, int* level);
Function scope_get_function(Scope scope, char* id, int* level);
int scope_get_level(Scope scope);

int scope_has_var(Scope scope, char* id);
int scope_has_function(Scope scope, char* id);

void scope_enter_subscope(Scope scope);
void scope_exit_subscope(Scope scope);

int scope_get_vars_size(Scope scope);

#endif
