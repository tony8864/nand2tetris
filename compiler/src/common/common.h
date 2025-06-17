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

<<<<<<< HEAD
=======
typedef struct RoutineVariable RoutineVariable;
typedef struct RoutineSymbolTable RoutineSymbolTable;
typedef struct RoutineSymbolTableEntry RoutineSymbolTableEntry;

>>>>>>> c0c37b5 (implement subroutine level symbol table)
typedef struct {
    char* currentClassName;
    char* currentFilePath;      // test/Main.jack
    char* currentFileName;      // Main.jack
    char* currentSourceName;    // Main

<<<<<<< HEAD
    ClassSymbolTable* classSymbolTable;
=======
    ClassSymbolTable*   classSymbolTable;
    RoutineSymbolTable* routineSymbolTable;
>>>>>>> c0c37b5 (implement subroutine level symbol table)
} CompilerContext;

extern CompilerContext gbl_context;

<<<<<<< HEAD
=======
#define ROUTINE_SYMTAB (gbl_context.routineSymbolTable)
#define CLASS_SYMTAB   (gbl_context.classSymbolTable)

>>>>>>> c0c37b5 (implement subroutine level symbol table)
typedef enum {
    STATIC_SCOPE,
    FIELD_SCOPE
} ClassScopeType;

typedef enum {
<<<<<<< HEAD
=======
    ARG_TYPE,
    VAR_TYPE
} RoutineScopeType;

typedef enum {
>>>>>>> c0c37b5 (implement subroutine level symbol table)
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

<<<<<<< HEAD
typedef struct ClassVarDecList {
    unsigned count;
    char** names;
} ClassVarDecList;
=======
typedef struct VarDecList {
    unsigned count;
    char** names;
} VarDecList;
>>>>>>> c0c37b5 (implement subroutine level symbol table)

VarType*
common_create_vartype(VarTypeKind kind, char* name);

VarType*
common_copy_vartype(VarType* original);

void
common_free_vartype(VarType* t);

char*
class_scope_type_to_string(ClassScopeType type);

char*
<<<<<<< HEAD
=======
routine_scope_type_to_string(RoutineScopeType type);

char*
>>>>>>> c0c37b5 (implement subroutine level symbol table)
var_type_to_string(VarType* type);

#endif