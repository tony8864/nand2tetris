%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "parser.tab.h"
#include "string_handler.h"

StringReader* strreader     = NULL;
unsigned      strstartline  = 0;

%}

%option noyywrap
%option yylineno


id              [a-zA-Z][a-zA-Z_0-9]*
integer         [0-9]+
space           [\t\n \t\v]
comment         "//".*

%x STRING_COND

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

"{"             { DEBUG_PRINT("punc: open curl\n");       return OPEN_CURLY; }
"}"             { DEBUG_PRINT("punc: close curl\n");      return CLOSE_CURLY; }
"("             { DEBUG_PRINT("punc: open par\n");        return OPEN_PAR; }
")"             { DEBUG_PRINT("punc: close par\n");       return CLOSE_PAR; }
"["             { DEBUG_PRINT("punc: open brack\n");      return OPEN_BRACKET; }
"]"             { DEBUG_PRINT("punc: close brack\n");     return CLOSE_BRACKET; }

"."             { DEBUG_PRINT("punc: dot\n");           return DOT; }
","             { DEBUG_PRINT("punc: comma\n");         return COMMA; }
";"             { DEBUG_PRINT("punc: semicolon\n");     return SEMICOLON; }

"+"             { DEBUG_PRINT("arithm: plus\n");        return PLUS; }
"-"             { DEBUG_PRINT("arithm: minus\n");       return MINUS; }
"*"             { DEBUG_PRINT("arithm: mul\n");         return MULT; }
"/"             { DEBUG_PRINT("arithm: div\n");         return DIV; }
"&"             { DEBUG_PRINT("arithm: and\n");         return AND; }
"|"             { DEBUG_PRINT("arithm: or\n");          return OR; }

">"             { DEBUG_PRINT("rel: gr\n");             return LESS; }
"<"             { DEBUG_PRINT("rel: ls\n");             return GREATER; }
"="             { DEBUG_PRINT("rel: eq\n");             return EQUAL; }
"~"             { DEBUG_PRINT("rel: ng\n");             return NEG; }


{id}            { DEBUG_PRINT("id: %s\n", yytext);  return IDENTIFIER; }
{integer}       { DEBUG_PRINT("int: %s\n", yytext); return INTEGER;}
{space}         { }
{comment}       { }

\"  {

    BEGIN(STRING_COND);
    strreader = strreader_initialize();
    strstartline = yylineno;
}

<STRING_COND>\" {
    char* string = strreader_getstring(strreader);
    printf("string : %s\n", string);

    free(strreader);
    strreader = NULL;
    BEGIN(INITIAL);

    yylval.strVal = string;
    return STRING;
}

<STRING_COND>.  {
    strreader_readchar(strreader, yytext[0]);
}

<STRING_COND><<EOF>>    {
    printf("Error: String at line %d not closed.\n", strstartline);
    exit(1);
}

.   { printf("Error at line %d: Did not find any match for \"%s\".\n", yylineno, yytext); exit(1); }

%%