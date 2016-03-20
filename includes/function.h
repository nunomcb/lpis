#ifndef FUNCTION_H
#define FUNCTION_H

#include "variable.h"
#include "list.h"

typedef struct Function_s* Function;

Function func_new(List args, Type return_type);
void func_destroy(Function f);

Type func_get_return_type(Function f);
List func_get_args(Function f);

int func_matches_args(Function f, List args);

#endif
