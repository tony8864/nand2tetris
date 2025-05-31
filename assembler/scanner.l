%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

%}

%option noyywrap
%option yylineno

id              [A-Za-z_.$:][A-Za-z0-9_.$:]*
integer         [0-9]+
space           [\t\n \t\v]
linecomment     "//".*

%%

"@"     { printf("at\n"); }
"("     { printf("open par\n"); }
")"     { printf("close par\n"); }
";"     { printf("semicolon\n"); }

"+"     { printf("plus\n"); }
"-"     { printf("minus\n"); }
"="     { printf("assign\n"); }

{linecomment}   { }
{space}         { }

{id}            { printf("id: %s\n", yytext); }
{integer}       { printf("int: %s\n", yytext); }

.               { printf("Did not find any match.\n"); }

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

    yylex();
    return 0;
}