#include "function.h"
#include <stdlib.h>

struct Function_s {
    List args;
    Type return_type;
};

Function func_new(List args, Type return_type) {
    Function f = (Function)malloc(sizeof(struct Function_s));

    f->args = args;
    f->return_type = return_type;

    return f;
}

void func_destroy(Function f) {

    list_destroy(f->args);
    free(f);
}

Type func_get_return_type(Function f) {
    return f->return_type; 
}

List func_get_args(Function f) {
    return f->args;
}

int func_matches_args(Function f, List args) {
    Type arg1, arg2;
    int i;

    if (list_size(f->args) != list_size(f->args))
        return 0;

    for (i = 0; i < list_size(f->args); i++) {
        arg1 = var_get_type((Variable)list_get_next(args));
        arg2 = var_get_type((Variable)list_get_next(f->args));

        if (arg1 != arg2)
            return 0;
    }


    return 1;
}
