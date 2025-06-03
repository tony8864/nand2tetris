%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

int yyerror(char* errorMsg);
int yylex(void);

extern FILE*    yyin;
extern int      yylineno;
%}


%start program

%union {
    int     intVal;
    char*   strVal;
    Segment_T segVal;
}

%token PUSH POP

%token<segVal> SEGMENT
%token<intVal> INTEGER

%%

program: lines;

lines:  lines line
        | %empty
        ;
    
line:   MEM_COMMAND
        ;

MEM_COMMAND: MEM_OP SEGMENT INTEGER  { printf("handle mem op\n"); };

MEM_OP: PUSH | POP;

%%

int main(int argc, char** argv) {
    if (argc == 2) {
        if (!(yyin = fopen(argv[1], "r"))) {
            printf("Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }
    else {
        printf("Usage: %s input.vm\n", argv[0]);
        return 1;
    }

    yyparse();

    return 0;
}

int yyerror(char* errorMsg) {
    printf("Syntax error at line %d with message: %s\n", yylineno, errorMsg);
    exit(1);
}