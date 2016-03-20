%{

#include <stdio.h>
#include <string.h>
#include "variable.h"
#include "list.h"
#include "parser.tab.h"

int level = 0;
int ret = 0;

int set_indent(char* tabs);
void close_indents();
%}

NUMBER      [-+]?[0-9]+
IDENTIFIER  ([a-zA-Z]+[0-9_]*)+
STRING      L?\"(\\.|[^\\"])*\"

%x indent

%option yylineno

%%

"if"            { return IF; }
"elseif"        { return ELSEIF; }
"else"          { return ELSE; }
"do"            { return DO; }
"while"         { return WHILE; }
"read"          { return READ; }
"write"         { return WRITE; }
"not"           { return NOT; }
"or"            { return OR; }
"and"           { return AND; }
"<="            { return LE; }
">="            { return GE; }
"=="            { return EQ; }
"eq"            { return EQ; }
"!="            { return NE; }
"not eq"        { return NE; }
"def"           { return DEF; }
"main"          { return MAIN; }
"return"        { return RETURN; }
"void"          { return VOID; }
"int"           { return INT; }
[ \t\n]         { ; }
[\+\-\*\/%]     { return yytext[0]; }
[<>=]           { return yytext[0]; }
[\[\]\(\)\{\}]  { return yytext[0]; }
[;:,]           { return yytext[0]; }
{NUMBER}        { yylval.num = atoi(yytext); return number; }
{STRING}        { yylval.str = strdup(yytext); return string; }
{IDENTIFIER}    { yylval.str = strdup(yytext); return identifier; }
.               { ; }

%%

int yywrap() {
    return 1;
}
