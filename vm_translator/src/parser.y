%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "function_codegen.h"
#include "compute_codegen.h"
#include "memory_codegen.h"
#include "label_codegen.h"
#include "codegen_util.h"
#include "parser_util.h"
#include "file_util.h"
#include "common.h"

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
    ComputeOp_T         computeVal;
    LabelOp_T           labelVal;
}

%token PUSH POP
%token FUNCTION CALL RETURN

%token<strVal>      IDENTIFIER
%token<intVal>      INTEGER

%token<segVal>      SEGMENT
%token<computeVal>  COMPUTE_CMD
%token<labelVal>    LABEL_CMD

%%

program: lines;

lines:  lines line
        | %empty
        ;
    
line:   MEMORY_COMMAND
        | COMPUTE_COMMAND
        | LABEL_COMMAND
        | FUNCTION_COMMAND
        | CALL_COMMAND
        | RETURN_COMMAND
        ;

MEMORY_COMMAND  :   POP SEGMENT INTEGER         { generate_memory_operation(OUT_FILE, MEM_POP, $2, $3);}
                    |PUSH SEGMENT INTEGER       { generate_memory_operation(OUT_FILE, MEM_PUSH, $2, $3);};

COMPUTE_COMMAND :   COMPUTE_CMD                 { generate_compute_operation(OUT_FILE, $1);}

LABEL_COMMAND   :   LABEL_CMD IDENTIFIER        { generate_label_operation(OUT_FILE, $1, $2);    free($2);}

FUNCTION_COMMAND:   FUNCTION IDENTIFIER INTEGER { generate_function_operation(OUT_FILE, $2, $3); free($2);}
CALL_COMMAND    :   CALL IDENTIFIER INTEGER     { generate_call_operation(OUT_FILE, $2, $3);      free($2);}
RETURN_COMMAND  :   RETURN                      { generate_return_operation(OUT_FILE);}

%%

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <input_folder>\n", argv[0]);
        return 1;
    }

    if (!fileUtil_isDirectory(argv[1])) {
        fprintf(stderr, "Error: Input path '%s' is not a directory.\n", argv[1]);
        return 1;
    }

    OUT_FOLDER = strdup("out");

    parse_vm_folder(argv[1]);
    
    vmparserUtil_combine_asm_files(argv[1]);
    vmparserUtil_append_bootstrap_code();

    return 0;
}

static void
parse_vm_folder(char* input_folder) {
    int count;
    char** files = fileUtil_get_files(input_folder, &count);
  
    for (int i = 0; i < count; i++) {
        parse_vm_file(files[i]);
        free(files[i]);
    }

    free(files);
}

static void
parse_vm_file(char* filename) {

    yyin = fopen(filename, "r");
    if (!yyin) {
        perror("fopen");
        exit(1);
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