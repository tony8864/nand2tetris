<<<<<<< HEAD
=======
#include "routine_symbol_table.h"
>>>>>>> c0c37b5 (implement subroutine level symbol table)
#include "class_symbol_table.h"
#include "parser_util.h"
#include "safe_util.h"
#include "common.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

extern int yylineno;

#define MAX_CLASS_VAR_DEC_LIST 20

static void
check_class_var_redeclared(char* name);

<<<<<<< HEAD
=======
static void
check_routine_var_redeclared(char* name);

>>>>>>> c0c37b5 (implement subroutine level symbol table)
void
parserutil_validate_class_name(char* className) {
    assert(gbl_context.currentSourceName);
    if (strcmp(className, gbl_context.currentSourceName) != 0) {
        printf("Error: Class name \"%s\" does not match file name.\n", className);
        exit(1);
    }
}

<<<<<<< HEAD
ClassVarDecList*
parserutil_create_class_var_list(char* name) {
    ClassVarDecList* list = safe_malloc(sizeof(ClassVarDecList));
=======
VarDecList*
parserutil_create_class_var_list(char* name) {
    VarDecList* list = safe_malloc(sizeof(VarDecList));
>>>>>>> c0c37b5 (implement subroutine level symbol table)
    char** names = safe_malloc(sizeof(char*) * MAX_CLASS_VAR_DEC_LIST);

    names[0]    = name;
    list->names = names;
    list->count = 1;

    return list;
}

void
<<<<<<< HEAD
parserutil_append_class_var(ClassVarDecList* list, char* name) {
=======
parserutil_append_var(VarDecList* list, char* name) {
>>>>>>> c0c37b5 (implement subroutine level symbol table)
    if (list->count == MAX_CLASS_VAR_DEC_LIST) {
        printf("Error at line %d: You can declare up to %d class variables in one line.\n", yylineno, MAX_CLASS_VAR_DEC_LIST);
        exit(1);
    }
    list->names[list->count++] = name;
}

void
<<<<<<< HEAD
parserutil_print_class_var_list(ClassVarDecList* list) {
=======
parserutil_print_var_list(VarDecList* list) {
>>>>>>> c0c37b5 (implement subroutine level symbol table)
    for (int i = 0; i < list->count; i++) {
        printf("(var: %s) ", list->names[i]);
    }
    printf("\n");
}

void
<<<<<<< HEAD
parserutil_free_class_var_list(ClassVarDecList* list) {
=======
parserutil_free_var_list(VarDecList* list) {
>>>>>>> c0c37b5 (implement subroutine level symbol table)
    for (int i = 0; i < list->count; i++) {
        free(list->names[i]);
    }
    free(list->names);
    free(list);
}

VarType*
parserutil_create_var_type(VarTypeKind kind, char* name) {
    VarType* type = safe_malloc(sizeof(VarType));
    type->kind = kind;
    type->className = name;
    return type;
}

void
<<<<<<< HEAD
parserutil_insert_class_variables(ClassScopeType kind, VarType* type, ClassVarDecList* list) {
=======
parserutil_insert_class_variables(ClassScopeType kind, VarType* type, VarDecList* list) {
>>>>>>> c0c37b5 (implement subroutine level symbol table)
    char* name;
    for (int i = 0; i < list->count; i++) {
        name = list->names[i];
        check_class_var_redeclared(name);
<<<<<<< HEAD
        classSymtab_insert(gbl_context.classSymbolTable, name, kind, type);
=======
        classSymtab_insert(CLASS_SYMTAB, name, kind, type);
    }
}

void
parserutil_insert_routine_variables(RoutineScopeType kind, VarType* type, VarDecList* list) {
    char* name;
    for (int i = 0; i < list->count; i++) {
        name = list->names[i];
        check_routine_var_redeclared(name);
        routineSymtab_insert(ROUTINE_SYMTAB, name, kind, type);
>>>>>>> c0c37b5 (implement subroutine level symbol table)
    }
}

static void
check_class_var_redeclared(char* name) {
<<<<<<< HEAD
    if (classSymtab_lookup(gbl_context.classSymbolTable, name) != NULL) {
=======
    if (classSymtab_lookup(CLASS_SYMTAB, name) != NULL) {
        printf("Error at line %d: Symbol \"%s\" is already declared.\n", yylineno, name);
        exit(1);
    }
}

static void
check_routine_var_redeclared(char* name) {
    if (routineSymtab_lookup(ROUTINE_SYMTAB, name) != NULL) {
>>>>>>> c0c37b5 (implement subroutine level symbol table)
        printf("Error at line %d: Symbol \"%s\" is already declared.\n", yylineno, name);
        exit(1);
    }
}