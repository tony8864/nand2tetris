%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"

%}

%option noyywrap
%option yylineno

id              [A-Za-z_.$:][A-Za-z0-9_.$:]*
integer         [0-9]+
space           [\t\n \t\v]
linecomment     "//".*

%%

"@"     { return AT; }
"("     { return OPEN_PARENTHESIS; }
")"     { return CLOSE_PARENTHESIS; }
";"     { return SEMICOLON; }
"="     { return ASSIGN; }

"DM"    { yylval.strVal = strdup("DM");  return DEST; }
"AM"    { yylval.strVal = strdup("AM");  return DEST; }
"AD"    { yylval.strVal = strdup("AD");  return DEST; }
"ADM"   { yylval.strVal = strdup("ADM"); return DEST; }

"!D"    { yylval.strVal = strdup("!D"); return COMP; }
"!M"    { yylval.strVal = strdup("!M"); return COMP; }
"!A"    { yylval.strVal = strdup("!A"); return COMP; }

"-D"    { yylval.strVal = strdup("-D"); return COMP; }
"-M"    { yylval.strVal = strdup("-M"); return COMP; }
"-A"    { yylval.strVal = strdup("-A"); return COMP; }

"D+1"    { yylval.strVal = strdup("D+1"); return COMP; }
"A+1"    { yylval.strVal = strdup("A+1"); return COMP; }
"M+1"    { yylval.strVal = strdup("M+1"); return COMP; }

"D-1"    { yylval.strVal = strdup("D-1"); return COMP; }
"A-1"    { yylval.strVal = strdup("A-1"); return COMP; }
"M-1"    { yylval.strVal = strdup("M-1"); return COMP; }

"D+A"    { yylval.strVal = strdup("D+A"); return COMP; }
"D-A"    { yylval.strVal = strdup("D-A"); return COMP; }
"A-D"    { yylval.strVal = strdup("A-D"); return COMP; }

"D+M"    { yylval.strVal = strdup("D+M"); return COMP; }
"D-M"    { yylval.strVal = strdup("D-M"); return COMP; }
"M-D"    { yylval.strVal = strdup("M-D"); return COMP; }

"D&A"    { yylval.strVal = strdup("D&A"); return COMP; }
"D|A"    { yylval.strVal = strdup("D|A"); return COMP; }

"D&M"    { yylval.strVal = strdup("D&M"); return COMP; }
"D|M"    { yylval.strVal = strdup("D|M"); return COMP; }

"A"     { yylval.strVal = strdup("A"); return REGISTER; }
"D"     { yylval.strVal = strdup("D"); return REGISTER; }
"M"     { yylval.strVal = strdup("M"); return REGISTER; }

"JGT"     { yylval.strVal = strdup("JGT"); return JMP; }
"JEQ"     { yylval.strVal = strdup("JEQ"); return JMP; }
"JGE"     { yylval.strVal = strdup("JGE"); return JMP; }
"JLT"     { yylval.strVal = strdup("JLT"); return JMP; }
"JNE"     { yylval.strVal = strdup("JNE"); return JMP; }
"JLE"     { yylval.strVal = strdup("JLE"); return JMP; }
"JMP"     { yylval.strVal = strdup("JMP"); return JMP; }

{linecomment}   { }
{space}         { }

{id}            { yylval.strVal = strdup(yytext); return IDENTIFIER; }
{integer}       { yylval.intVal = atoi(yytext); return INTEGER; }

.               { printf("Did not find any match.\n"); }

%%