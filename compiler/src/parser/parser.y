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
    int             intVal;
    char*           strVal;
    ClassScopeType  classScopeType;
    VarType         varType;
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

%type<classScopeType>   classScope
%type<varType>          varType


%%

program
    : CLASS IDENTIFIER OPEN_CURLY optionalClassVarDecl CLOSE_CURLY
        {
            BISON_DEBUG_PRINT("class definition: %s\n", $2);
        }
    ;

optionalClassVarDecl
                    : classVarDeclarations
                    | %empty
                    ;


classVarDeclarations
                : classVarDeclaration
                | classVarDeclarations classVarDeclaration
                ;

classVarDeclaration
                : classScope varType classVariables SEMICOLON
                ;
                
classVariables
            : IDENTIFIER
            | classVariables COMMA IDENTIFIER
            ;
            
varType
    : INT
        {
            $$ = (VarType){ .kind = INT_TYPE,    .className = NULL };
        }
    | CHAR
        {
            $$ = (VarType){ .kind = CHAR_TYPE,   .className = NULL };
        }
    | BOOLEAN
        {
            $$ = (VarType){ .kind = BOOLEAN_TYPE, .className = NULL };
        }
    | IDENTIFIER
        {
            $$ = (VarType){ .kind = CLASS_TYPE,   .className = $1 };
        }
    ;


classScope
        : STATIC
            {
                $$ = STATIC_SCOPE;
            }
        | FIELD
            {
                $$ = FIELD_SCOPE;
            }
        ;


%%

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage: %s <input_folder>\n", argv[0]);
        return 1;
    }

    common_set_lex_debug_mode(0);
    common_set_bison_debug_mode(1);

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