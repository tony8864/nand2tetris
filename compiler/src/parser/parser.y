%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "class_symbol_table.h"
#include "parser_util.h"
#include "file_util.h"
#include "safe_util.h"
#include "str_util.h"
#include "common.h"

int yyerror(char* msg);
int yylex(void);

static void
parse_file(char* filename);

static void
set_global_context_for_file(char* filepath);

static void
set_global_context_for_symtables();

static void
free_global_context();

extern FILE*    yyin;
extern int      yylineno;

%}

%union {
    int                 intVal;
    char*               strVal;
    ClassScopeType      classScopeType;
    VarType*            varType;
    SubroutineType      subroutine;
    ClassVarDecList*    classVarDecList;
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

%type<strVal>           className varName
%type<classScopeType>   classScope
%type<varType>          varType
%type<subroutine>       subroutine
%type<classVarDecList>  classVariables

%%

program
    : CLASS className OPEN_CURLY optionalClassVarDecl optionalSubroutineDecl CLOSE_CURLY
        {
            BISON_DEBUG_PRINT("\tclass definition: %s\n\n", $2);
            free($2);
        }
    ;

className
        : IDENTIFIER
            {
                parserutil_validate_class_name($1);
                $$ = $1;
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
                    {
                        parserutil_insert_class_variables($1, $2, $3);
                        parserutil_free_class_var_list($3);
                        common_free_vartype($2);
                    }
                ;
                
classVariables
            : varName
                {
                    $$ = parserutil_create_class_var_list($1);
                }
            | classVariables COMMA varName
                {
                    parserutil_append_class_var($1, $3);
                }
            ;

varName
            : IDENTIFIER
                {
                    $$ = $1;
                }
            ;

varType
    : INT
        {
            $$ = parserutil_create_var_type(INT_TYPE, NULL);
        }
    | CHAR
        {
            $$ = parserutil_create_var_type(CHAR_TYPE, NULL);
        }
    | BOOLEAN
        {
            $$ = parserutil_create_var_type(BOOLEAN_TYPE, NULL);
        }
    | IDENTIFIER
        {
            $$ = parserutil_create_var_type(CLASS_TYPE, $1);
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


subroutine
        : CONSTRUCTOR
            {
                $$ = CONSTRUCTOR_TYPE;
            }
        | FUNCTION
            {
                $$ = FUNCTION_TYPE;
            }
        | METHOD
            {
                $$ = METHOD_TYPE;
            }
        ;

returnType
        : VOID
        | varType
        ;

optionalSubroutineDecl
                    : subroutineDeclarations
                    | %empty
                    ;

subroutineDeclarations
                    : subroutineDeclaration
                    | subroutineDeclarations subroutineDeclaration
                    ;

subroutineDeclaration
                    : subroutine returnType subroutineName OPEN_PAR parameterList CLOSE_PAR subroutineBody
                    ;

subroutineName
            : IDENTIFIER
            ;

parameterList
            : parameters
            | %empty
            ;

parameters
        : param
        | parameters COMMA param
        ;

param
    : varType IDENTIFIER
    ;

subroutineBody
            : OPEN_CURLY optionalLocalVarDecl statements CLOSE_CURLY
            ;

optionalLocalVarDecl
                    : localVarDeclarations
                    | %empty
                    ;

localVarDeclarations
                    : localVarDeclaration
                    | localVarDeclarations localVarDeclaration
                    ;

localVarDeclaration
                : VAR varType localVariables SEMICOLON
                ;

localVariables
            : varName
            | localVariables COMMA varName
            ;

statements
        : statements statement
        | %empty
        ;

statement
        : ifstatement
        | letstatement
        | whilestatement
        | dostatement
        | returnstatement
        ;

ifstatement
        : IF OPEN_PAR expression CLOSE_PAR OPEN_CURLY statements CLOSE_CURLY optionalelse
        ;

optionalelse
            : ELSE OPEN_CURLY statements CLOSE_CURLY
            | %empty
            ;

letstatement
            : LET varName optionalArrayExpression EQUAL expression SEMICOLON
            ;

whilestatement
            : WHILE OPEN_PAR expression CLOSE_PAR OPEN_CURLY statements CLOSE_CURLY
            ;

dostatement
        : DO subroutineCall SEMICOLON
        ;

returnstatement
            : RETURN opetionalReturnExpression SEMICOLON
            ;

opetionalReturnExpression
                        : expression
                        | %empty
                        ;

optionalArrayExpression
                    : OPEN_BRACKET expression CLOSE_BRACKET
                    | %empty
                    ;

expression
        : term optionalTerm
        ;

optionalTerm
            : optionalTerm operationTerm
            | %empty
            ;

operationTerm
            : operation term
            ;

term
    : INTEGER
    | STRING
    | keywordConst
    | varName
    | varName OPEN_BRACKET expression CLOSE_BRACKET
    | OPEN_PAR expression CLOSE_PAR
    | unaryOperation term
    | subroutineCall
    ;

subroutineCall
            : subroutineName OPEN_PAR optionalExpressionList CLOSE_PAR
            | className DOT subroutineName  OPEN_PAR optionalExpressionList CLOSE_PAR
            | varName DOT subroutineName    OPEN_PAR optionalExpressionList CLOSE_PAR
            ;

optionalExpressionList
                    : expressionList
                    | %empty
                    ;

expressionList
            : expressions
            ;

expressions
        : expression
        | expressions COMMA expression
        ;

operation
        : PLUS
        | MINUS
        | MULT
        | DIV
        | AND
        | OR
        | LESS
        | GREATER
        | EQUAL
        ;

unaryOperation
            : MINUS
            | NEG
            ;

keywordConst
            : TRUE
            | FALSE
            | THIS
            | NULL_T
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
        parse_file(files[i]);
    }

    FILEUTIL_free_file_list(files, count);

    return 0;
}

static void
parse_file(char* filepath) {
    printf("\n\tfile: %s\n", filepath);

    set_global_context_for_symtables();
    set_global_context_for_file(filepath);

    yyin = safe_fopen(filepath, "r");
    yylineno = 1;

    yyparse();
    fclose(yyin);

    classSymtab_print(gbl_context.classSymbolTable);
    
    free_global_context();
    classSymtab_free(gbl_context.classSymbolTable);
}

static void
set_global_context_for_file(char* filepath) {
    gbl_context.currentFilePath   = safe_strdup(filepath);
    gbl_context.currentSourceName = strutil_path_to_source_name(filepath);
}

static void
set_global_context_for_symtables() {
    gbl_context.classSymbolTable = classSymtab_initialize();
}

static void
free_global_context() {
    free(gbl_context.currentFilePath);
    free(gbl_context.currentSourceName);
}

int
yyerror(char* errorMsg) {
    printf("Syntax error at line %d with message: %s\n", yylineno, errorMsg);
    exit(1);
}