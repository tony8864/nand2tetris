%{

#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"

%}

%option noyywrap
%option yylineno

integer         -?[0-9]+
space           [\t\n \t\v]
linecomment     "//".*

%%

"push"  { return PUSH; }
"pop"   { return POP; }

"argument"  { yylval.segVal = SEG_ARGUMENT; return SEGMENT; }
"local"     { yylval.segVal = SEG_LOCAL;    return SEGMENT; }
"static"    { yylval.segVal = SEG_STATIC;   return SEGMENT; }
"this"      { yylval.segVal = SEG_THIS;     return SEGMENT; }
"that"      { yylval.segVal = SEG_THAT;     return SEGMENT; }

{linecomment}   { }
{space}         { }

{integer}       { yylval.intVal = atoi(yytext); return INTEGER; }

.               { printf("Did not find any match.\n"); }

%%