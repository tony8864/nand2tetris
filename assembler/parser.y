%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "./parser_util/parser_util.h"

int yyerror(char *errorMsg);
int yylex(void);

extern FILE*    yyin;
extern int      yylineno;
    
%}

%start program

%union {
    int intVal;
    char* strVal;
}

%token AT
%token OPEN_PARENTHESIS CLOSE_PARENTHESIS
%token SEMICOLON
%token ASSIGN

%token<strVal> IDENTIFIER
%token<intVal> INTEGER

%token<strVal> DEST COMP JMP
%token<strVal> REGISTER

%type<strVal> DESTINATION COMPUTATION

%%

program: lines;

lines:  lines line
        | %empty
        ;

line:   A_COMMAND
        | C_COMMAND
        | L_COMMAND
        ;

A_COMMAND:  AT IDENTIFIER   { parserUtil_handle_A_command_id($2); free($2); }
            | AT INTEGER    { parserUtil_handle_A_command_int($2); }
            ;

C_COMMAND:  DESTINATION ASSIGN COMPUTATION SEMICOLON JMP    { parserUtil_handle_C_command_type1($3, $1, $5); free($1); free($3); free($5); }
            | DESTINATION ASSIGN COMPUTATION                { parserUtil_handle_C_command_type2($3, $1); free($1); free($3); }
            | COMPUTATION SEMICOLON JMP                     { parserUtil_handle_C_command_type3($1, $3); free($1); free($3); }
            ;

L_COMMAND:  OPEN_PARENTHESIS IDENTIFIER CLOSE_PARENTHESIS   { parserUtil_handle_L_command($2, yylineno); free($2); }

DESTINATION:    DEST        { $$ = $1; }
                | REGISTER  { $$ = $1; }
                ;

COMPUTATION:    COMP        { $$ = $1; }
                | REGISTER  { $$ = $1; }
                | INTEGER
                    {
                        if ($1 == 0 || $1 == 1 || $1 == -1) {
                            char buffer[3];
                            snprintf(buffer, sizeof(buffer), "%d", $1);
                            $$ = strdup(buffer);
                        }
                        else {
                            yyerror("Only -1, 0, 1 are valid constants in computation");
                        }
                    }
                ;

%%


int main(int argc, char** argv) {

    if (argc == 3) {
        if (!(yyin = fopen(argv[1], "r"))) {
            printf("Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }
    else {
        printf("Usage: %s input.asm output.hack\n", argv[0]);
        return 1;
    }

    parserUtil_initialize();
    parserUtil_setOutputFile(argv[2]);

    parserUtil_setFirstPass();
    yyparse();

    rewind(yyin);

    parserUtil_setSecondPass();
    yyparse();

    parserUtil_cleanup();

    return 0;
}

int yyerror(char* errorMsg) {
    printf("Syntax error at line %d with message: %s\n", yylineno, errorMsg);
    exit(1);
}