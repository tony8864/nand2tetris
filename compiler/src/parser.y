%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "file_util.h"
#include "safe_util.h"

int yyerror(char* msg);
int yylex(void);

extern FILE*    yyin;
extern int      yylineno;

%}

%start program

%token CLASS CONSTRUCTOR FUNCTION METHOD FIELD STATIC
%token VAR INT CHAR BOOLEAN VOID TRUE FALSE NULL_T THIS
%token LET DO IF ELSE WHILE RETURN

%%

program: keyword_list
        | %empty
        ;

keyword_list: KEYWORD
              | keyword_list KEYWORD
              ;  

KEYWORD:
      CLASS
    | CONSTRUCTOR
    | FUNCTION
    | METHOD
    | FIELD
    | STATIC
    | VAR
    | INT
    | CHAR
    | BOOLEAN
    | VOID
    | TRUE
    | FALSE
    | NULL_T
    | THIS
    | LET
    | DO
    | IF
    | ELSE
    | WHILE
    | RETURN
    ;


%%

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage: %s <input_folder>\n", argv[0]);
        return 1;
    }

    common_set_debug_mode(1);

    char* input_folder = argv[1];

    if (!FILEUTIL_is_directory(input_folder)) {
        printf("Error: input is not a valid folder.\n");
        return 1;
    }

    int     count;
    char**  files = FILEUTIL_list_files(input_folder, &count);

    for (int i = 0; i < count; i++) {
        printf("file: %s\n", files[i]);
        yyin = safe_fopen(files[i], "r");
        yyparse();
        fclose(yyin);
    }

    FILEUTIL_free_file_list(files, count);

    return 0;
}

int
yyerror(char* errorMsg) {
    printf("Syntax error at line %d with message: %s\n", yylineno, errorMsg);
    exit(1);
}