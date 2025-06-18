#include "routine_symbol_table.h"
#include "class_symbol_table.h"
#include "parser_util.h"
#include "safe_util.h"
#include "common.h"
#include "emitter.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

extern int yylineno;

#define MAX_CLASS_VAR_DEC_LIST 20

static void
check_class_var_redeclared(char* name);

static void
check_routine_var_redeclared(char* name);

static void
free_param(Param* p);

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

ParamList*
parserutil_create_param_list() {
    ParamList* list = safe_malloc(sizeof(ParamList));
    list->count = 0;
    list->head = NULL;
    return list;
}

Param*
parserutil_create_param(VarType* type, char* name) {
    Param* p = safe_malloc(sizeof(Param));
    p->type = type;
    p->name = safe_strdup(name);
    return p;
}

void
parserutil_append_param(ParamList* list, Param* param) {
    param->next = list->head;
    list->head = param;
    list->count++;
}

void
parserutil_free_param_list(ParamList* list) {
    Param* curr = list->head;
    Param* next = NULL;
    while (curr) {
        next = curr->next;
        free_param(curr);
        curr = next;
    }
    free(list);
}

void
parserutil_print_param_list(ParamList* list) {
    Param* p = list->head;
    while (p) {
        printf("(name: %s, type: %s) ", p->name, common_var_type_to_string(p->type));
        p = p->next;
    }
    printf("\n");
}

void
parserutil_insert_parameters(ParamList* list) {
    Param* p = list->head;
    char* name;
    VarType* type;
    while (p) {
        name = p->name;
        type = p->type;
        check_routine_var_redeclared(name);
        routineSymtab_insert(ROUTINE_SYMTAB, name, ARG_TYPE, type);
        p = p->next;
    }
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

Term*
parserutil_create_int_term(int int_val) {
    Term* t = safe_malloc(sizeof(Term));
    t->type = INT_TERM;
    t->value.int_val = int_val;
    return t;
}

Term*
parserutil_create_var_term(char* name) {
    Term* t = safe_malloc(sizeof(Term));
    t->type = VAR_TERM;
    t->value.var_val = strdup(name);
    return t;
}

Term*
parserutil_create_grouped_term(Expression* e) {
    Term* t = safe_malloc(sizeof(Term));
    t->type = GROUPED_TERM;
    t->value.expr_val = e;
    return t;
}

OpTerm*
parserutil_create_op_term(OperationType op, Term* term) {
    OpTerm* opterm = safe_malloc(sizeof(OpTerm));
    opterm->op = op;
    opterm->term = term;
    return opterm;
}

Expression*
parserutil_create_expression(Term* term, OpTerm* opTerm) {
    Expression* e = safe_malloc(sizeof(Expression));
    e->term = term;
    e->rest = opTerm;
    return e;
}

OpTerm*
parserutil_append_op_term(OpTerm* list, OpTerm* node) {
    if (!list) return node;

    OpTerm* cur = list;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = node;
    return list;
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

static void
free_param(Param* p) {
    common_free_vartype(p->type);
    free(p->name);
    free(p);
}