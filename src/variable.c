#include "variable.h"
#include <stdlib.h>
#include <string.h>

struct Variable_s {
    char* name;
    Type type;
    int offset;
    int size;
};

Variable var_new(char* name, Type type, int offset, int size) {
    Variable var;
    
    if (type == VOID)
        return NULL;

    var = (Variable)malloc(sizeof(struct Variable_s));

    if (name)
        var->name = strdup(name);
    else
        var->name = NULL;

    var->type = type;
    var->offset = offset;
    var->size = size;

    return var;
}

void var_destroy(Variable var) {
    if (var->name) free(var->name);
    free(var);
}

int var_get_offset(Variable var) {
    return var->offset;
}

Type var_get_type(Variable var) {
    return var->type;
}

char* var_get_name(Variable var) {
    return var->name;
}

int var_get_size(Variable var) {
    return var->size;
}

void var_set_offset(Variable var, int offset) {
    var->offset = offset;
}

int var_is_array(Variable var) {
    return var->type == ARRAY;
}

int var_is_int(Variable var) {
    return var->type == INTEGER;
}
