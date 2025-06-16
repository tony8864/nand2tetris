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

void
parserutil_validate_class_name(char* className) {
    assert(gbl_context.currentSourceName);
    if (strcmp(className, gbl_context.currentSourceName) != 0) {
        printf("Error: Class name \"%s\" does not match file name.\n", className);
        exit(1);
    }
}

ClassVarDecList*
parserutil_create_class_var_list(char* name) {
    ClassVarDecList* list = safe_malloc(sizeof(ClassVarDecList));
    char** names = safe_malloc(sizeof(char*) * MAX_CLASS_VAR_DEC_LIST);

    names[0]    = name;
    list->names = names;
    list->count = 1;

    return list;
}

void
parserutil_append_class_var(ClassVarDecList* list, char* name) {
    if (list->count == MAX_CLASS_VAR_DEC_LIST) {
        printf("Error at line %d: You can declare up to %d class variables in one line.\n", yylineno, MAX_CLASS_VAR_DEC_LIST);
        exit(1);
    }
    list->names[list->count++] = name;
}

void
parserutil_print_class_var_list(ClassVarDecList* list) {
    for (int i = 0; i < list->count; i++) {
        printf("(var: %s) ", list->names[i]);
    }
    printf("\n");
}

void
parserutil_free_class_var_list(ClassVarDecList* list) {
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
parserutil_insert_class_variables(ClassScopeType kind, VarType* type, ClassVarDecList* list) {
    char* name;
    for (int i = 0; i < list->count; i++) {
        name = list->names[i];
        check_class_var_redeclared(name);
        classSymtab_insert(gbl_context.classSymbolTable, name, kind, type);
    }
}

static void
check_class_var_redeclared(char* name) {
    if (classSymtab_lookup(gbl_context.classSymbolTable, name) != NULL) {
        printf("Error at line %d: Symbol \"%s\" is already declared.\n", yylineno, name);
        exit(1);
    }
}