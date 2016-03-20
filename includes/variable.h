#ifndef VARIABLE_H
#define VARIABLE_H

typedef enum {VOID, INTEGER, ARRAY} Type;

typedef struct Variable_s* Variable;

Variable var_new(char* name, Type type, int offset, int size);
void var_destroy(Variable var);

int var_get_offset(Variable var);
Type var_get_type(Variable var);
int var_get_size(Variable var);
char* var_get_name(Variable var);

void var_set_offset(Variable var, int offset);

int var_is_array(Variable var);
int var_is_int(Variable var);

#endif
