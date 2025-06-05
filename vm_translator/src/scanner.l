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

"local"     { yylval.segVal = SEG_LOCAL;    return SEGMENT; }
"argument"  { yylval.segVal = SEG_ARGUMENT; return SEGMENT; }
"this"      { yylval.segVal = SEG_THIS;     return SEGMENT; }
"that"      { yylval.segVal = SEG_THAT;     return SEGMENT; }
"pointer"   { yylval.segVal = SEG_POINTER;  return SEGMENT; }
"temp"      { yylval.segVal = SEG_TEMP;     return SEGMENT; }
"constant"  { yylval.segVal = SEG_CONSTANT; return SEGMENT; }
"static"    { yylval.segVal = SEG_STATIC;   return SEGMENT; }

"add"       { yylval.computeVal = ADD_CMD;  return COMPUTE_CMD; }
"sub"       { yylval.computeVal = SUB_CMD;  return COMPUTE_CMD; }
"neg"       { yylval.computeVal = NEG_CMD;  return COMPUTE_CMD; }
"eq"        { yylval.computeVal = EQ_CMD;   return COMPUTE_CMD; }
"gt"        { yylval.computeVal = GT_CMD;   return COMPUTE_CMD; }
"lt"        { yylval.computeVal = LT_CMD;   return COMPUTE_CMD; }
"and"       { yylval.computeVal = AND_CMD;  return COMPUTE_CMD; }
"or"        { yylval.computeVal = OR_CMD;   return COMPUTE_CMD; }
"not"       { yylval.computeVal = NOT_CMD;  return COMPUTE_CMD; }

{linecomment}   { }
{space}         { }

{integer}       { yylval.intVal = atoi(yytext); return INTEGER; }

.               { printf("Error at line %d: Did not find any match for \"%s\".\n", yylineno, yytext); exit(1); }

%%