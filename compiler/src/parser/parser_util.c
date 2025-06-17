#include "routine_symbol_table.h"
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

static void
check_routine_var_redeclared(char* name);

void
parserutil_validate_class_name(char* className) {
    assert(gbl_context.currentSourceName);
    if (strcmp(className, gbl_context.currentSourceName) != 0) {
        printf("Error: Class name \"%s\" does not match file name.\n", className);
        exit(1);
    }
}

VarDecList*
parserutil_create_class_var_list(char* name) {
    VarDecList* list = safe_malloc(sizeof(VarDecList));
    char** names = safe_malloc(sizeof(char*) * MAX_CLASS_VAR_DEC_LIST);

    names[0]    = name;
    list->names = names;
    list->count = 1;

    return list;
}

void
parserutil_append_var(VarDecList* list, char* name) {
    if (list->count == MAX_CLASS_VAR_DEC_LIST) {
        printf("Error at line %d: You can declare up to %d class variables in one line.\n", yylineno, MAX_CLASS_VAR_DEC_LIST);
        exit(1);
    }
    list->names[list->count++] = name;
}

void
parserutil_print_var_list(VarDecList* list) {
    for (int i = 0; i < list->count; i++) {
        printf("(var: %s) ", list->names[i]);
    }
    printf("\n");
}

void
parserutil_free_var_list(VarDecList* list) {
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
parserutil_insert_class_variables(ClassScopeType kind, VarType* type, VarDecList* list) {
    char* name;
    for (int i = 0; i < list->count; i++) {
        name = list->names[i];
        check_class_var_redeclared(name);
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
    }
}

static void
check_class_var_redeclared(char* name) {
    if (classSymtab_lookup(CLASS_SYMTAB, name) != NULL) {
        printf("Error at line %d: Symbol \"%s\" is already declared.\n", yylineno, name);
        exit(1);
    }
}

static void
check_routine_var_redeclared(char* name) {
    if (routineSymtab_lookup(ROUTINE_SYMTAB, name) != NULL) {
        printf("Error at line %d: Symbol \"%s\" is already declared.\n", yylineno, name);
        exit(1);
    }
}