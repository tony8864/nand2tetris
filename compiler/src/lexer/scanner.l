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

"class"         { LEX_DEBUG_PRINT("keyword: class\n");      return CLASS; }
"constructor"   { LEX_DEBUG_PRINT("keyword: constructor\n");return CONSTRUCTOR; }
"function"      { LEX_DEBUG_PRINT("keyword: function\n");   return FUNCTION; }
"method"        { LEX_DEBUG_PRINT("keyword: method\n");     return METHOD; }
"field"         { LEX_DEBUG_PRINT("keyword: field\n");      return FIELD; }
"static"        { LEX_DEBUG_PRINT("keyword: static\n");     return STATIC; }
"var"           { LEX_DEBUG_PRINT("keyword: var\n");        return VAR; }
"int"           { LEX_DEBUG_PRINT("keyword: int\n");        return INT; }
"char"          { LEX_DEBUG_PRINT("keyword: char\n");       return CHAR; }
"boolean"       { LEX_DEBUG_PRINT("keyword: boolean\n");    return BOOLEAN; }
"void"          { LEX_DEBUG_PRINT("keyword: void\n");       return VOID; }
"true"          { LEX_DEBUG_PRINT("keyword: true\n");       return TRUE; }
"false"         { LEX_DEBUG_PRINT("keyword: false\n");      return FALSE; }
"null"          { LEX_DEBUG_PRINT("keyword: null\n");       return NULL_T; }
"this"          { LEX_DEBUG_PRINT("keyword: this\n");       return THIS; }
"let"           { LEX_DEBUG_PRINT("keyword: let\n");        return LET; }
"do"            { LEX_DEBUG_PRINT("keyword: do\n");         return DO; }
"if"            { LEX_DEBUG_PRINT("keyword: if\n");         return IF; }
"else"          { LEX_DEBUG_PRINT("keyword: else\n");       return ELSE; }
"while"         { LEX_DEBUG_PRINT("keyword: while\n");      return WHILE; }
"return"        { LEX_DEBUG_PRINT("keyword: return\n");     return RETURN; }

"{"             { LEX_DEBUG_PRINT("punc: open curl\n");       return OPEN_CURLY; }
"}"             { LEX_DEBUG_PRINT("punc: close curl\n");      return CLOSE_CURLY; }
"("             { LEX_DEBUG_PRINT("punc: open par\n");        return OPEN_PAR; }
")"             { LEX_DEBUG_PRINT("punc: close par\n");       return CLOSE_PAR; }
"["             { LEX_DEBUG_PRINT("punc: open brack\n");      return OPEN_BRACKET; }
"]"             { LEX_DEBUG_PRINT("punc: close brack\n");     return CLOSE_BRACKET; }

"."             { LEX_DEBUG_PRINT("punc: dot\n");           return DOT; }
","             { LEX_DEBUG_PRINT("punc: comma\n");         return COMMA; }
";"             { LEX_DEBUG_PRINT("punc: semicolon\n");     return SEMICOLON; }

"+"             { LEX_DEBUG_PRINT("arithm: plus\n");        return PLUS; }
"-"             { LEX_DEBUG_PRINT("arithm: minus\n");       return MINUS; }
"*"             { LEX_DEBUG_PRINT("arithm: mul\n");         return MULT; }
"/"             { LEX_DEBUG_PRINT("arithm: div\n");         return DIV; }
"&"             { LEX_DEBUG_PRINT("arithm: and\n");         return AND; }
"|"             { LEX_DEBUG_PRINT("arithm: or\n");          return OR; }

">"             { LEX_DEBUG_PRINT("rel: gr\n");             return LESS; }
"<"             { LEX_DEBUG_PRINT("rel: ls\n");             return GREATER; }
"="             { LEX_DEBUG_PRINT("rel: eq\n");             return EQUAL; }
"~"             { LEX_DEBUG_PRINT("rel: ng\n");             return NEG; }


{id}            { LEX_DEBUG_PRINT("id: %s\n", yytext);  yylval.strVal = strdup(yytext); return IDENTIFIER; }
{integer}       { LEX_DEBUG_PRINT("int: %s\n", yytext); yylval.intVal = atoi(yytext);   return INTEGER;}
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