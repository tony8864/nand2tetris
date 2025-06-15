#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

extern unsigned lex_debug_mode;
extern unsigned bison_debug_mode;

#define LEX_DEBUG_PRINT(fmt, ...) \
            do { if (lex_debug_mode) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

#define BISON_DEBUG_PRINT(fmt, ...) \
            do { if (bison_debug_mode) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)

void
common_set_lex_debug_mode(unsigned enabled);

void
common_set_bison_debug_mode(unsigned enabled);


typedef enum {
    STATIC_SCOPE,
    FIELD_SCOPE
} ClassScopeType;

typedef enum {
    INT_TYPE,
    CHAR_TYPE,
    BOOLEAN_TYPE,
    CLASS_TYPE
} VarTypeKind;

typedef struct {
    VarTypeKind kind;
    char* className;
} VarType;

typedef enum {
    CONSTRUCTOR_TYPE,
    FUNCTION_TYPE,
    METHOD_TYPE
} SubroutineType;

#endif