%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "routine_symbol_table.h"
#include "class_symbol_table.h"
#include "parser_util.h"
#include "file_util.h"
#include "safe_util.h"
#include "str_util.h"
#include "common.h"
#include "emitter.h"

int yyerror(char* msg);
int yylex(void);

static void
set_debugging();

static void
parse_file(char* filename);

static void
prepare_compilation(char* filepath);

static void
finalize_compilation();

static void
set_lex_globals();

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
    VarDecList*         VarDecList;
    ParamList*          paramList;
    Param*              param;
    OperationType       opType;
    Term*               term;
    OpTerm*             opTerm;
    Expression*         expression;
    ExpressionList*     expressionList;
    SubroutineCall*     subroutineCall;
    UnaryOperationType  unaryOp;
    KeywordConstType    keywordConstType;
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

%type<strVal>           className varName subroutineName
%type<classScopeType>   classScope
%type<varType>          varType returnType subroutineHeader
%type<subroutine>       subroutine
%type<VarDecList>       classVariables localVariables
%type<paramList>        parameterList parameters
%type<param>            param
%type<opType>           operation
%type<term>             term
%type<opTerm>           operationTerm optionalTerm
%type<expression>       expression opetionalReturnExpression optionalArrayExpression
%type<expressionList>   expressionList optionalExpressionList
%type<unaryOp>          unaryOperation
%type<subroutineCall>   subroutineCall directcall methodcall
%type<keywordConstType> keywordConst

%%

program
    : CLASS className OPEN_CURLY optionalClassVarDecl optionalSubroutineDecl CLOSE_CURLY
        {
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
                        parserutil_free_var_list($3);
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
                    parserutil_append_var($1, $3);
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
            {
                $$ = NULL;
            }
        | varType
            {
                $$ = $1;
            }
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
                    : subroutineHeader OPEN_PAR parameterList CLOSE_PAR subroutineBody
                        {
                            if ($3 != NULL) {
                                parserutil_free_param_list($3);
                            }
                            routineSymtab_print(ROUTINE_SYMTAB);
                            routineSymtab_free(ROUTINE_SYMTAB);
                            common_free_vartype($1);
                        }
                    ;

subroutineHeader
                : subroutine returnType subroutineName
                    {
                        ROUTINE_SYMTAB = routineSymtab_initialize();
                        if ($1 == METHOD_TYPE) {
                            VarType* type = common_create_vartype(CLASS_TYPE, SRC_NAME);
                            routineSymtab_insert(ROUTINE_SYMTAB, "this", ARG_TYPE, type);
                            free(type);
                        }
                        else
                        if ($1 == CONSTRUCTOR_TYPE) {
                            if ($2 == NULL) {
                                printf("[Error]: %s:%d: Return type of constructor cannot be void.\n", FULL_SRC_PATH, yylineno);
                                exit(1);
                            }
                           
                            char* returnTypeName = common_get_classname_from_type($2);
                            if (returnTypeName == NULL) {
                                printf("[Error]: %s:%d: Return type of constructor must match class name.\n", FULL_SRC_PATH, yylineno);
                                exit(1);
                            }

                            if (strcmp(returnTypeName, SRC_NAME) != 0) {
                                printf("[Error]: %s:%d: Return type of constructor must match class name.\n", FULL_SRC_PATH, yylineno);
                                exit(1);
                            }
                        }
                
                        CURRENT_SUBROUTINE = parserutil_create_subroutine($1, $2, $3);
                        parserutil_insert_subroutine(CURRENT_SUBROUTINE);
                        $$ = $2;
                        free($3);
                    }
                ;

subroutineName
            : IDENTIFIER
                {
                    $$ = $1;
                }
            ;

parameterList
            : parameters
                {
                    $$ = $1;
                    parserutil_insert_parameters($1);
                }
            | %empty
                {
                    $$ = NULL;
                }
            ;

parameters
        : param
            {
                $$ = parserutil_create_param_list();
                parserutil_append_param($$, $1);
            }
        | parameters COMMA param
            {
                parserutil_append_param($1, $3);
            }
        ;

param
    : varType IDENTIFIER
        {
            $$ = parserutil_create_param($1, $2);
            free($2);
        }
    ;

subroutineBody
            : OPEN_CURLY optionalLocalVarDecl
                {
                    emitter_generate_subroutine();
                }
              statements CLOSE_CURLY
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
                    {
                        parserutil_insert_routine_variables(VAR_TYPE, $2, $3);
                        parserutil_free_var_list($3);
                        common_free_vartype($2);
                    }
                ;

localVariables
            : varName
                {
                    $$ = parserutil_create_class_var_list($1);
                }
            | localVariables COMMA varName
                {
                    parserutil_append_var($1, $3);
                }
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
        : ifExpression CLOSE_PAR OPEN_CURLY statements 
            {
                emitter_generate_after_if_statements();
            }
          CLOSE_CURLY optionalelse
            {
                emitter_generate_after_optionalElse();
            }
        ;

ifExpression
    : IF OPEN_PAR expression
        {
            emitter_generate_if_expression($3);
            parserutil_free_if_expression($3);
        }
    ;

optionalelse
            : ELSE OPEN_CURLY statements CLOSE_CURLY
            | %empty
            ;

letstatement
            : LET varName optionalArrayExpression EQUAL expression SEMICOLON
                {
                    
                    if ($3 != NULL) {
                        emitter_generate_array_let_statement($2, $3, $5);
                        parserutil_free_array_let_statement($2, $3, $5);
                    }
                    else {
                        emitter_generate_let_statement($2, $5);
                        parserutil_free_let_statement($2, $5);
                    }   
                }
            ;

whilestatement
            : whileExpression CLOSE_PAR OPEN_CURLY statements CLOSE_CURLY
                {
                    emitter_generate_after_while_statements();
                }
            ;

whileExpression
            : WHILE OPEN_PAR expression
                {
                    emitter_generate_while_expression($3);
                    parserutil_free_while_expression($3);
                }
            ;

dostatement
        : DO subroutineCall SEMICOLON
            {
                emitter_generate_do_statement($2);
                parserutil_free_do_statement($2);
            }
        ;

returnstatement
            : RETURN opetionalReturnExpression SEMICOLON
                {
                    emitter_generate_return_statement($2);
                    parser_util_free_return_statement($2);
                }
            ;

opetionalReturnExpression
                        : expression
                            {
                                $$ = $1;
                            }
                        | %empty
                            {
                                $$ = NULL;
                            }
                        ;

optionalArrayExpression
                    : OPEN_BRACKET expression CLOSE_BRACKET
                        {
                            $$ = $2;
                        }
                    | %empty
                        {
                            $$ = NULL;
                        }
                    ;

expression
        : term optionalTerm
            {   
                $$ = parserutil_create_expression($1, $2);
            }
        ;

optionalTerm
            : optionalTerm operationTerm
                {
                    $$ = parserutil_append_op_term($1, $2);
                }
            | %empty
                {
                    $$ = NULL;
                }
            ;

operationTerm
            : operation term
                {   
                    $$ = parserutil_create_op_term($1, $2);   
                }
            ;

term
    : INTEGER
        {
            $$ = parserutil_create_int_term($1);
        }
    | STRING
        {
            $$ = parserutil_create_string_term($1);
            free($1);
        }
    | keywordConst
        {
            $$ = parserutil_create_keyword_const_term($1);
        }
    | varName
        {
            $$ = parserutil_create_var_term($1);
            free($1);
        }
    | varName OPEN_BRACKET expression CLOSE_BRACKET
        {
            $$ = parserutil_create_array_term($1, $3);
            free($1);
        }
    | OPEN_PAR expression CLOSE_PAR
        {   
            $$ = parserutil_create_grouped_term($2);
        }
    | unaryOperation term
        {
            $$ = parserutil_create_unary_term($1, $2);
        }
    | subroutineCall
        {
            $$ = parserutil_create_subroutine_term($1);
        }
    ;

subroutineCall
            : directcall
                {
                    $$ = $1;
                }
            | methodcall
                {
                    $$ = $1;
                }
            ;

directcall
        : subroutineName OPEN_PAR optionalExpressionList CLOSE_PAR
            {   
                $$ = parserutil_create_direct_call($1, $3);
            }
        ;


methodcall
        : IDENTIFIER DOT subroutineName OPEN_PAR optionalExpressionList CLOSE_PAR
            {
                $$ = parserutil_create_method_call($1, $3, $5);
                free($1);
                free($3);
            }
        ;

optionalExpressionList
                    : expressionList
                        {
                            $$ = $1;
                        }
                    | %empty
                        {
                            $$ = NULL;
                        }
                    ;

expressionList
        : expression
            {
                $$ = parserutil_create_expressionList($1);
            }
        | expressionList COMMA expression
            {
                $$ = parserutil_append_expression($1, $3);
            }
        ;

operation
        : PLUS
            {
                $$ = PLUS_OP;
            }
        | MINUS
            {
                $$ = MINUS_OP;
            }
        | MULT
            {
                $$ = UNDEFINED;
            }
        | DIV
            {
                $$ = UNDEFINED;
            }
        | AND
            {
                $$ = AND_OP;
            }
        | OR
            {
                $$ = OR_OP;
            }
        | LESS
            {
                $$ = LESS_OP;
            }
        | GREATER
            {
                $$ = GREATER_OP;
            }
        | EQUAL
            {
                $$ = EQUAL_OP;
            }
        ;

unaryOperation
            : MINUS
                {
                    $$ = UNARY_MINUS;
                }
            | NEG
                {
                    $$ = UNARY_NEG;
                }
            ;

keywordConst
            : TRUE
                {
                    $$ = KEYWORD_TRUE;
                }
            | FALSE
                {
                    $$ = KEYWORD_FALSE;
                }
            | THIS
                {
                    $$ = KEYWORD_THIS;
                }
            | NULL_T
                {
                    $$ = KEYWORD_NULL;
                }
            ;

%%

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage: %s <input_folder>\n", argv[0]);
        return 1;
    }

    set_debugging();

    char* input_folder = argv[1];
    SRC_FOLDER = strutil_strip_trailing_slash(input_folder);
    OUT_FOLDER = strdup("out");

    if (!FILEUTIL_is_directory(input_folder)) {
        printf("Error: input is not a valid folder.\n");
        return 1;
    }

    int     count;
    char**  files = FILEUTIL_list_files(input_folder, &count);

    FILEUTIL_collect_jack_class_names(files, count);
    FILEUTIL_print_jack_class_names();

    for (int i = 0; i < count; i++) {
        parse_file(files[i]);
    }

    FILEUTIL_free_file_list(files, count);
    FILEUTIL_free_jack_class_names(count);

    free(SRC_FOLDER);
    free(OUT_FOLDER);

    return 0;
}

static void
set_debugging() {
    common_set_lex_debug_mode(0);
    common_set_bison_debug_mode(0);
}

static void
parse_file(char* filepath) {
    printf("\n\tfile: %s\n", filepath);
    prepare_compilation(filepath);
    yyparse();
    finalize_compilation();
}

static void
prepare_compilation(char* filepath) {
    set_global_context_for_symtables();
    set_global_context_for_file(filepath);
    set_lex_globals();
    FILEUTIL_create_vm_file();
}

static void
finalize_compilation() {
    classSymtab_print(CLASS_SYMTAB);

    fclose(yyin);
    fclose(VM_FILE);
    free_global_context();
}

static void
set_lex_globals() {
    yyin = safe_fopen(FULL_SRC_PATH, "r");
    yylineno = 1;
}

static void
set_global_context_for_file(char* filepath) {
    FULL_SRC_PATH   = safe_strdup(filepath);
    SRC_NAME        = strutil_path_to_source_name(filepath);
}

static void
set_global_context_for_symtables() {
    CLASS_SYMTAB = classSymtab_initialize();
}

static void
free_global_context() {
    classSymtab_free(CLASS_SYMTAB);
    free(FULL_SRC_PATH);
    free(SRC_NAME);
    free(VM_NAME);
}

int
yyerror(char* errorMsg) {
    printf("Syntax error at line %d with message: %s\n", yylineno, errorMsg);
    exit(1);
}