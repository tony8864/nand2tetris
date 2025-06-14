#ifndef STR_LEX_HANDLER_H
#define STR_LEX_HANDLER_H

typedef struct StringReader StringReader;

StringReader*
strreader_initialize();

void
strreader_readchar(StringReader* reader, char c);

char*
strreader_getstring(StringReader* reader);

#endif