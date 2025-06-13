%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "parser.tab.h"

%}

%option noyywrap
%option yylineno

space           [\t\n \t\v]

%%

"class"         { DEBUG_PRINT("keyword: class\n");      return CLASS; }
"constructor"   { DEBUG_PRINT("keyword: constructor\n");return CONSTRUCTOR; }
"function"      { DEBUG_PRINT("keyword: function\n");   return FUNCTION; }
"method"        { DEBUG_PRINT("keyword: method\n");     return METHOD; }
"field"         { DEBUG_PRINT("keyword: field\n");      return FIELD; }
"static"        { DEBUG_PRINT("keyword: static\n");     return STATIC; }
"var"           { DEBUG_PRINT("keyword: var\n");        return VAR; }
"int"           { DEBUG_PRINT("keyword: int\n");        return INT; }
"char"          { DEBUG_PRINT("keyword: char\n");       return CHAR; }
"boolean"       { DEBUG_PRINT("keyword: boolean\n");    return BOOLEAN; }
"void"          { DEBUG_PRINT("keyword: void\n");       return VOID; }
"true"          { DEBUG_PRINT("keyword: true\n");       return TRUE; }
"false"         { DEBUG_PRINT("keyword: false\n");      return FALSE; }
"null"          { DEBUG_PRINT("keyword: null\n");       return NULL_T; }
"this"          { DEBUG_PRINT("keyword: this\n");       return THIS; }
"let"           { DEBUG_PRINT("keyword: let\n");        return LET; }
"do"            { DEBUG_PRINT("keyword: do\n");         return DO; }
"if"            { DEBUG_PRINT("keyword: if\n");         return IF; }
"else"          { DEBUG_PRINT("keyword: else\n");       return ELSE; }
"while"         { DEBUG_PRINT("keyword: while\n");      return WHILE; }
"return"        { DEBUG_PRINT("keyword: return\n");     return RETURN; }

{space}         { }

.               { printf("Error at line %d: Did not find any match for \"%s\".\n", yylineno, yytext); exit(1); }

%%