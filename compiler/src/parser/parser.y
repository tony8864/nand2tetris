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

%union {
    int intVal;
    char* strVal;
}

%start program

%token CLASS CONSTRUCTOR FUNCTION METHOD FIELD STATIC
%token VAR INT CHAR BOOLEAN VOID TRUE FALSE NULL_T THIS
%token LET DO IF ELSE WHILE RETURN

%token OPEN_CURLY CLOSE_CURLY OPEN_PAR CLOSE_PAR OPEN_BRACKET CLOSE_BRACKET
%token DOT COMMA SEMICOLON
%token PLUS MINUS MULT DIV AND OR
%token LESS GREATER EQUAL NEG

%token<intVal> INTEGER
%token<strVal> STRING
%token<strVal> IDENTIFIER

%%

program: token_seq;

token_seq:
        token_seq token
        | %empty
        ;

token: keyword_list
        | punc_list
        | arithm_list
        | rel_list
        | id_list
        | int_list
        | str_list
        ;

keyword_list: keyword
              | keyword_list keyword
              ;

punc_list: punctuation
            | punc_list punctuation
            ;

arithm_list: arithm
            | arithm_list arithm
            ;

rel_list: rel
          | rel_list rel
          ;

id_list: IDENTIFIER
            | id_list IDENTIFIER
            ;

int_list:  INTEGER
            | int_list INTEGER
            ;

str_list:  STRING   { free($1); }
            | str_list STRING { free($2); }
            ;

keyword:
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


punctuation:
        OPEN_CURLY
        | CLOSE_CURLY
        | OPEN_PAR
        | CLOSE_PAR
        | OPEN_BRACKET
        | CLOSE_BRACKET
        | DOT
        | COMMA
        | SEMICOLON
        ;

arithm:
    PLUS
    | MINUS
    | MULT
    | DIV
    | AND
    | OR
    ;

rel: 
    LESS
    | GREATER
    | EQUAL
    | NEG
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
        yylineno = 1;
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