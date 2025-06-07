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

static void
parse_vm_folder(char* path);

static void
parse_vm_file(char* filename);

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
    if (argc != 2) {
        printf("Usage: %s input.vm | folder\n", argv[0]);
        return 1;
    }

    unsigned isDirectory = fileUtil_isDirectory(argv[1]);

    if (isDirectory) {
        parse_vm_folder(argv[1]);
        vmparserUtil_combine_asm_files(argv[1]);
    }
    else {
        parse_vm_file(argv[1]);
    }

    return 0;
}

static void
parse_vm_folder(char* path) {
    int count;
    char** files = fileUtil_get_files(path, &count);
    
    for (int i = 0; i < count; i++) {
        parse_vm_file(files[i]);
        free(files[i]);
    }

    free(files);
}

static void
parse_vm_file(char* filename) {

    if ((yyin = fopen(filename, "r")) != NULL) {
        if (!yyin) {
            perror("fopen");
            exit(1);
        }
    }
    
    vmparserUtil_open_out_file(filename);
    yyparse();
    vmparserUtil_cleanup();
    fclose(yyin);
}

int
yyerror(char* errorMsg) {
    printf("Syntax error at line %d with message: %s\n", yylineno, errorMsg);
    exit(1);
}