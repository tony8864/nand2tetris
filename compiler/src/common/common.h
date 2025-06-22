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

typedef struct RoutineVariable RoutineVariable;
typedef struct RoutineSymbolTable RoutineSymbolTable;
typedef struct RoutineSymbolTableEntry RoutineSymbolTableEntry;

typedef struct Subroutine Subroutine;

typedef struct {
    char** jack_class_names;
    unsigned jack_classes_count;

    char* currentClassName;
    char* currentFilePath;      // test/Main.jack
    char* currentFileName;      // Main.jack
    char* currentSourceName;    // Main
    char* sourceFolder;         // test

    char* outputFolderName;
    char* currentVmFilename;    // Main.vm
    FILE* vm_file;

    ClassSymbolTable*   classSymbolTable;
    RoutineSymbolTable* routineSymbolTable;

    Subroutine* currentRoutine;
} CompilerContext;

extern CompilerContext gbl_context;

#define JACK_CLASSES        (gbl_context.jack_class_names)
#define JACK_CLASSES_COUNT  (gbl_context.jack_classes_count)

#define CURRENT_SUBROUTINE  (gbl_context.currentRoutine)

#define ROUTINE_SYMTAB  (gbl_context.routineSymbolTable)
#define CLASS_SYMTAB    (gbl_context.classSymbolTable)

#define OUT_FOLDER      (gbl_context.outputFolderName)
#define FULL_SRC_PATH   (gbl_context.currentFilePath)
#define SRC_FOLDER      (gbl_context.sourceFolder)
#define SRC_NAME        (gbl_context.currentSourceName)
#define VM_NAME         (gbl_context.currentVmFilename)
#define VM_FILE         (gbl_context.vm_file)

typedef enum {
    STATIC_SCOPE,
    FIELD_SCOPE
} ClassScopeType;

typedef enum {
    ARG_TYPE,
    VAR_TYPE
} RoutineScopeType;

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

typedef struct VarDecList {
    unsigned count;
    char** names;
} VarDecList;

typedef struct Param {
    VarType* type;
    char* name;
    struct Param* next;
} Param;

typedef struct ParamList {
    unsigned count;
    Param* head;
} ParamList;

typedef struct Term Term;
typedef struct OpTerm OpTerm;
typedef struct Expression Expression;
typedef struct ExpressionList ExpressionList;
typedef struct SubroutineCall SubroutineCall;

typedef enum {
    PLUS_OP,
    MINUS_OP,
    AND_OP,
    OR_OP,
    LESS_OP,
    GREATER_OP,
    EQUAL_OP,
    UNDEFINED
} OperationType;

typedef enum {
    INT_TERM,
    VAR_TERM,
    GROUPED_TERM,
    UNARY_TERM,
    SUBROUTINE_TERM
} TermType;

typedef enum {
    UNARY_MINUS,
    UNARY_NEG
} UnaryOperationType;

VarType*
common_create_vartype(VarTypeKind kind, char* name);

VarType*
common_copy_vartype(VarType* original);

char*
common_get_classname_from_type(VarType* type);

void
common_free_vartype(VarType* t);

char*
common_var_type_to_string(VarType* type);

unsigned
common_is_class_name(char* name);

#endif