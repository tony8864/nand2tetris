#include "string_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "safe_util.h"

#define MAX_SIZE 256

typedef struct StringReader {
    char string[MAX_SIZE];
    unsigned int len;
} StringReader;

StringReader*
strreader_initialize() {
    StringReader* reader = safe_malloc(sizeof(StringReader));
    memset(reader->string, 0, MAX_SIZE);
    reader->len = 0;
    return reader;
}

void
strreader_readchar(StringReader* reader, char c) {
    reader->string[reader->len++] = c;
}

char*
strreader_getstring(StringReader* reader) {
    return safe_strdup(reader->string);
}