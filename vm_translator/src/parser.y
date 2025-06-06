%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#include "file_util.h"
#include "parser_util.h"

int yyerror(char* errorMsg);
int yylex(void);

extern FILE*    yyin;
extern int      yylineno;

%}


%start program

%union {
    int                 intVal;
    char*               strVal;
    Segment_T           segVal;
    ComputeCommand_T    computeVal;
}

%token PUSH POP

%token<intVal>      INTEGER
%token<segVal>      SEGMENT
%token<computeVal>  COMPUTE_CMD

%%

program: lines;

lines:  lines line
        | %empty
        ;
    
line:   MEM_COMMAND
        | CMP_COMMAND
        ;

MEM_COMMAND:  POP SEGMENT INTEGER   { vmparserUtil_handleMemoryOperation(MEM_POP, $2, $3); }; 
            | PUSH SEGMENT INTEGER  { vmparserUtil_handleMemoryOperation(MEM_PUSH, $2, $3); };
            ; 

CMP_COMMAND: COMPUTE_CMD { vmparserUtil_handleComputeOperation($1); }


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

    vmparserUtil_open_out_file(argv[1]);
    yyparse();
    vmparserUtil_cleanup();

    return 0;
}

int yyerror(char* errorMsg) {
    printf("Syntax error at line %d with message: %s\n", yylineno, errorMsg);
    exit(1);
}