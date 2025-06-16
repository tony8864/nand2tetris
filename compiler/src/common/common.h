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

typedef struct ClassVariable ClassVariable;
typedef struct ClassSymbolTable ClassSymbolTable;
typedef struct ClassSymbolTableEntry ClassSymbolTableEntry;

typedef struct {
    char* currentClassName;
    char* currentFilePath;      // test/Main.jack
    char* currentFileName;      // Main.jack
    char* currentSourceName;    // Main

    ClassSymbolTable* classSymbolTable;
} CompilerContext;

extern CompilerContext gbl_context;

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

typedef struct ClassVarDecList {
    unsigned count;
    char** names;
} ClassVarDecList;

VarType*
common_create_vartype(VarTypeKind kind, char* name);

VarType*
common_copy_vartype(VarType* original);

void
common_free_vartype(VarType* t);

char*
class_scope_type_to_string(ClassScopeType type);

char*
var_type_to_string(VarType* type);

#endif