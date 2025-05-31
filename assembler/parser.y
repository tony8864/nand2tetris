%{

#include <stdio.h>

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

%%

program: lines;

lines:  lines line
        | %empty
        ;

line:   A_COMMAND       { printf("a command\n"); }
        | C_COMMAND     { printf("c command\n"); }
        | L_COMMAND     { printf("l command\n"); }
        ;

A_COMMAND:  AT IDENTIFIER   { printf("at id\n"); }
            | AT INTEGER    { printf("at int\n"); }
            ;

C_COMMAND:  DESTINATION ASSIGN COMPUTATION SEMICOLON JMP    { printf("dest = comp ; jmp\n"); }
            | DESTINATION ASSIGN COMPUTATION                { printf("dest = comp\n"); }
            | COMPUTATION SEMICOLON JMP                     { printf("comp ; jmp\n"); }
            ;

L_COMMAND:  OPEN_PARENTHESIS IDENTIFIER CLOSE_PARENTHESIS;  

DESTINATION:    DEST
                | REGISTER
                ;

COMPUTATION:    COMP
                | REGISTER
                ;

%%


int main(int argc, char** argv) {

    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            printf("Cannot read file: %s\n", argv[1]);
            return 1;
        }
    }
    else {
        yyin = stdin;
    }

    yyparse();
    return 0;
}

int yyerror(char* errorMsg) {
    printf("Syntax error at line %d with message: %s\n", yylineno, errorMsg);
}