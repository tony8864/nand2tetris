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
    SubroutineType  subroutine;
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

%%

program
    : CLASS className OPEN_CURLY optionalClassVarDecl optionalSubroutineDecl CLOSE_CURLY
        {
            BISON_DEBUG_PRINT("class definition: %s\n", $2);
        }
    ;

className
        : IDENTIFIER
            {
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
                ;
                
classVariables
            : varName
            | classVariables COMMA varName
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
        
        }
    | CHAR
        {
            
        }
    | BOOLEAN
        {
           
        }
    | IDENTIFIER
        {
            
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