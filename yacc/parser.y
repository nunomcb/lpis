%{

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "list.h"
#include "variable.h"
#include "function.h"
#include "scope.h"

#define ERROR_EXIT(s) yyerror(s);\
                      fclose(output_file);\
                      exit(1)

#define PRINT(s, rest...) fprintf(output_file, s "\n", ## rest)
#define GET_LABEL() label_counter++
#define PRINT_LABEL(n) fprintf(output_file, "L%d:\n", n)
#define FUNC_LABEL_PREFIX "f"
#define PRINT_FUNC_LABEL(name) fprintf(output_file, "f_%s:\n", name)
#define JZ(n) fprintf(output_file, "JZ L%d\n", n)
#define JUMP(n) fprintf(output_file, "JUMP L%d\n", n)
#define RETURN_VAR "#ret"
#define DEFAULT_FILE_NAME "out.vm"

extern int yylineno;
extern int yylex();

void yyerror(char* s);

char* output_file_name = NULL;
FILE* output_file = NULL;
int label_counter = 0;

Scope scope;

void enter_subscope();
void exit_subscope();

void store(Variable v);

Variable get_var(char* name, int offset);

%}

%token number string identifier

%token MAIN
%token IF ELSEIF ELSE
%token DO WHILE
%token INDENT UNINDENT NEWLINE
%token READ WRITE ERROR
%token NOT
%token OR AND
%token EQ NE GE LE
%token DEF RETURN
%token VOID INT ARRAY

%union {
    int num;
    char* str;
    List list;
    Type type;
}

%type <num> number
%type <str> identifier
%type <str> string
%type <list> Parameters
%type <list> Arguments
%type <type> Assignment
%type <type> Expression
%type <type> FunctionCall
%type <type> ArithmeticExpression
%type <type> Type
%type <type> Var

%left '='

%left OR
%left AND
%left EQ NE
%left '<' LE '>' GE
%left '+' '-'
%left '*' '/' '%'
%right NOT

%%

Program
    :   Declarations {
            PRINT("START");
            PRINT("PUSHA " FUNC_LABEL_PREFIX "_main");
            PRINT("CALL");
        } FunctionDefinitions MainFunction
    ;

Block : '{' { enter_subscope(); } Declarations Statements { exit_subscope(); } '}'
      ;

FunctionBlock : '{' { enter_subscope(); } Declarations Statements Return { exit_subscope(); } '}'
              ;

Return : RETURN { push_var_addr(RETURN_VAR); } Expression { PRINT("STOREN"); } ';'
       ;

Statements :
           | Statements Statement
           ;

Statement : Assignment ';'
          | FunctionCall ';' { PRINT("POP 1"); }
          | Write ';'
          | Read ';'
          | If
          | While
          | DoWhile ';'
          ;

Write : WRITE Expression    { PRINT("WRITEI")                       ; }
      | WRITE string        { PRINT("PUSHS %s", $2); PRINT("WRITES"); }
      ;

Read : READ { PRINT("READ"); PRINT("ATOI"); }
     ;

Type : INT          { $$ = INTEGER; }
     | INT '[' ']'  { $$ = ARRAY; }
     ;

Declarations :
             | Declarations Declaration
             ;

Declaration : INT identifier ';'                { decl_var($2);       }
            | INT '[' number ']' identifier ';' { decl_array($5, $3); }
            ;

Assignment  : Var '=' Expression { PRINT("STOREN"); }
            ;

Var : identifier { push_var_addr($1); $$ = var_type($1); }
    | identifier { push_array_addr($1); $$ = INTEGER; } '[' Expression ']'
    ;


FunctionDefinitions : { }
                    | FunctionDefinitions FunctionDefinition { }
                    ;

FunctionDefinition
    :   DEF identifier {
            PRINT_FUNC_LABEL($2);
        }  '(' Parameters ')' ':' INT {
            def_function($2, $5, INTEGER);
        }
        FunctionBlock {
            exit_subscope();
            PRINT("RETURN");
        }
    ;

MainFunction : DEF MAIN { PRINT_FUNC_LABEL("main"); } '(' ')' ':' VOID Block { PRINT("STOP"); }
             ;

Parameters
    :   Type identifier {
            $$ = list_new(NULL);
            Variable v = var_new($2, $1, 0, 1);
            list_add($$, v);
        }
    |   Parameters ',' Type identifier {
            $$ = $1;
            Variable v = var_new($4, $3, 0, 1);
            list_add($$, v);
        }
    ;

FunctionCall : identifier '(' Arguments ')' { $$ = call_function($1, $3);   }
             | identifier '(' ')'           { $$ = call_function($1, NULL); }
             ;

Arguments
    :   Expression {
            $$ = list_new(__destroy_var);
            Variable v = var_new(NULL, $1, 0, 0);
            list_add($$, v);
      }
    |   Arguments ',' Expression {
            $$ = $1;
            Variable v = var_new(NULL, $3, 0, 0);
            list_add($$, v);
        }
    ;

Expression  : '(' Expression ')'   { $$ = $2;                             }
            | ArithmeticExpression { $$ = INTEGER;                        }
            | LogicExpression      { $$ = INTEGER;                        }
            | Var                  { PRINT("LOADN"); $$ = $1;             }
            | number               { PRINT("PUSHI %d", $1); $$ = INTEGER; }
            | FunctionCall         { $$ = $1;                             }
            | Assignment           { $$ = $1;                             }
            | Read                 { $$ = INTEGER;                        }
            ;

ArithmeticExpression : Expression '+' Expression { PRINT("ADD"); }
                     | Expression '-' Expression { PRINT("SUB"); }
                     | Expression '*' Expression { PRINT("MUL"); }
                     | Expression '/' Expression { PRINT("DIV"); }
                     | Expression '%' Expression { PRINT("MOD"); }
                     ;

LogicExpression : Expression AND Expression { PRINT("AND");     }
                | Expression OR Expression  { PRINT("OR");      }
                | Expression EQ Expression  { PRINT("EQ");      }
                | Expression NE Expression  { PRINT("EQ\nNOT"); }
                | Expression '>' Expression { PRINT("SUP");     }
                | Expression GE Expression  { PRINT("SUPEQ");   }
                | Expression '<' Expression { PRINT("INF");     }
                | Expression LE Expression  { PRINT("INFEQ");   }
                | NOT Expression            { PRINT("NOT");     }
                ;

If :
    IF Expression {
        int lbl = GET_LABEL();
        JZ(lbl);
        $<num>$ = lbl;
    }
    Block {
        int lbl = GET_LABEL();
        JUMP(lbl);
        PRINT_LABEL($<num>3);
        $<num>$ = lbl;
    }
    ELSE Block {
        PRINT_LABEL($<num>5);
    }
   ;

While :
    WHILE {
        int lbl = GET_LABEL();
        PRINT_LABEL(lbl);
        $<num>$ = lbl;
    }
    Expression {
        int lbl = GET_LABEL();
        JZ(lbl);
        $<num>$ = lbl;
    }
    Block {
        JUMP($<num>2);
        PRINT_LABEL($<num>4);
    }
      ;

DoWhile :
    DO {
        int lbl = GET_LABEL();
        PRINT_LABEL(lbl);
        $<num>$ = lbl;
    }
    Block WHILE Expression {
        PRINT("NOT");
        JZ($<num>2);
    }
        ;

%%

void yyerror(char* str) {
    printf("\x1B[31mERROR\x1B[0m @ \x1B[36m%d\x1B[0m: \n\t%s\n", yylineno, str);
}

void enter_subscope() {
    scope_enter_subscope(scope);
}

void exit_subscope() {
    PRINT("POP %d", scope_get_vars_size(scope));
    scope_exit_subscope(scope);
}

void __destroy_var(void* var) {
    var_destroy((Variable)var);
}

void push_array_addr(char* name) {
    Variable v;
    int level;

    v = scope_get_var(scope, name, &level);

    if (!v) {
        ERROR_EXIT("variable undefined.");
    }

    if (var_get_type(v) != ARRAY) {
        ERROR_EXIT("variable is not an array.");
    }

    if (level == 1) {
        PRINT("PUSHG %d", var_get_offset(v));
    }
    else {
        PRINT("PUSHL %d", var_get_offset(v));
    }
}

void push_var_addr(char* name) {
    Variable v;
    int level;

    level = 0;

    v = scope_get_var(scope, name, &level);

    if (!v) {
        ERROR_EXIT("variable undefined.");
    }

    if (level == 1) {
        PRINT("PUSHGP");
    }
    else {
        PRINT("PUSHFP");
    }

    PRINT("PUSHI %d", var_get_offset(v));
}

void decl_array(char* name, int size) {
    Variable v;
    int offset, r;

    offset = scope_get_vars_size(scope) + 1;
    v = var_new(name, ARRAY, offset, size + 1);

    r = scope_def_var(scope, name, v);

    if (r == 1) {
        ERROR_EXIT("variable is already defined in the current scope.");
    }

    PRINT("PUSHFP");
    PRINT("PUSHI %d", offset + 1);
    PRINT("ADD");
    PRINT("PUSHN %d", size);
}

void decl_var(char* name) {
    Variable v;
    int offset, r;

    offset = scope_get_vars_size(scope) + 1;
    v = var_new(name, INTEGER, offset, 1);

    r = scope_def_var(scope, name, v);

    if (r == 1) {
        ERROR_EXIT("variable is already defined in the current scope.");
    }

    PRINT("PUSHN 1");
}

Type var_type(char* name) {
    int level;
    Variable v = scope_get_var(scope, name, &level);

    if (!v) {
        ERROR_EXIT("variable is undefined!");
    }

    return var_get_type(v);
}


void def_function(char* name, List args, Type return_type) {
    int r, i, size;
    Function f;
    Variable v;

    size = list_size(args);

    f = func_new(args, return_type);
    r = scope_def_function(scope, name, f);

    if (r == 1) {
        ERROR_EXIT("multiple definitions of function.");
    }

    enter_subscope();

    i = 0;

    while ((v = list_get_next(args))) {
        var_set_offset(v, - size + i);
        scope_def_var(scope, var_get_name(v), v);
        i++;
    }
}

Type call_function(char* name, List args) {
    int level;
    Function f = scope_get_function(scope, name, &level);
    Variable ret = scope_get_var(scope, RETURN_VAR, &level);

    if (!f) {
        ERROR_EXIT("function is undefined.");
    }

    if (!func_matches_args(f, args)) {
        ERROR_EXIT("the arguments don't match the function's signature.");
    }

    list_destroy(args);

    PRINT("PUSHA " FUNC_LABEL_PREFIX "_%s", name);
    PRINT("CALL");
    PRINT("PUSHG %d", var_get_offset(ret));

    return func_get_return_type(f);
}

int main(int argc, char* argv[]) {
    int i;

    for (i = 0; i < argc; i++) {
        if (!strcmp("-o", argv[i])) {
            i++;
            
            if (i == argc) {
                printf("-o option used without filename. " DEFAULT_FILE_NAME " will be used");
            }
            else {
                output_file_name = argv[i];
            }

            break;
        }
    }

    if (output_file_name == NULL) {
        output_file_name = DEFAULT_FILE_NAME;
    }

    output_file = fopen(output_file_name, "w");

    
    printf("hello");
    scope = scope_new();
    scope_enter_subscope(scope);

    decl_var(RETURN_VAR);

    yyparse();

    fclose(output_file);
    scope_destroy(scope);

    return 0;
}
