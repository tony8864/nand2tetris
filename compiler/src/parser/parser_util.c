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
check_if_name_is_valid_class(char* name);

static void
free_param(Param* p);

static void
free_expression(Expression* e);

static void
free_term(Term* t);

static void
free_opterm(OpTerm* opt);

static void
free_subroutine_call(SubroutineCall* call);

static void
free_expression_list(ExpressionList* list);

static void
free_unary_term(UnaryTerm* t);

// -----------------------------------------------------------------------------
// Param List Utilities
// -----------------------------------------------------------------------------

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
    param->next = NULL;

    if (list->head == NULL) {
        list->head = param;
    }
    else {
        Param* cur = list->head;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = param;
    }
    list->count++;
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

// -----------------------------------------------------------------------------
// Variable Declaration Utilities
// -----------------------------------------------------------------------------

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
parserutil_insert_class_variables(ClassScopeType kind, VarType* type, VarDecList* list) {
    char* name;
    for (int i = 0; i < list->count; i++) {
        name = list->names[i];
        check_class_var_redeclared(name);
        classSymtab_insert_variable(CLASS_SYMTAB, name, kind, type);
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

void
parserutil_free_var_list(VarDecList* list) {
    for (int i = 0; i < list->count; i++) {
        free(list->names[i]);
    }
    free(list->names);
    free(list);
}

void
parserutil_print_var_list(VarDecList* list) {
    for (int i = 0; i < list->count; i++) {
        printf("(var: %s) ", list->names[i]);
    }
    printf("\n");
}

// -----------------------------------------------------------------------------
// Term Utilities
// -----------------------------------------------------------------------------

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
    t->value.var_val = safe_strdup(name);
    return t;
}

Term*
parserutil_create_grouped_term(Expression* e) {
    Term* t = safe_malloc(sizeof(Term));
    t->type = GROUPED_TERM;
    t->value.expr_val = e;
    return t;
}

Term*
parserutil_create_subroutine_term(SubroutineCall* call) {
    Term* t = safe_malloc(sizeof(Term));
    t->type = SUBROUTINE_TERM;
    t->value.call_val = call;
    return t;
}

Term*
parserutil_create_unary_term(UnaryOperationType op, Term* term) {
    Term* t = safe_malloc(sizeof(Term));
    UnaryTerm* ut = safe_malloc(sizeof(UnaryTerm));
    ut->term = term;
    ut->op = op;
    t->value.unary_val = ut;
    t->type = UNARY_TERM;
    return t;
}

Term*
parserutil_create_keyword_const_term(KeywordConstType type) {
    Term* t = safe_malloc(sizeof(Term));
    t->type = KEYWORDCONST_TERM;
    t->value.keywordconst_val = type;
    return t;
}

Term*
parserutil_create_string_term(char* str) {
    Term* t = safe_malloc(sizeof(Term));
    t->type = STRING_TERM;
    t->value.str_val = strdup(str);
    return t;
}

// -----------------------------------------------------------------------------
// Operators and Expressions
// -----------------------------------------------------------------------------

OpTerm*
parserutil_create_op_term(OperationType op, Term* term) {
    OpTerm* opterm = safe_malloc(sizeof(OpTerm));
    opterm->op = op;
    opterm->term = term;
    opterm->next = NULL;
    return opterm;
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

Expression*
parserutil_create_expression(Term* term, OpTerm* opTerm) {
    Expression* e = safe_malloc(sizeof(Expression));
    e->term = term;
    e->rest = opTerm;
    return e;
}

// -----------------------------------------------------------------------------
// Expression Lists
// -----------------------------------------------------------------------------

ExpressionList*
parserutil_create_expressionList(Expression* expr) {
    ExpressionList* list = safe_malloc(sizeof(ExpressionList));
    list->expr = expr;
    list->next = NULL;
    return list;
}

ExpressionList*
parserutil_append_expression(ExpressionList* list, Expression* e) {
    ExpressionList* node = parserutil_create_expressionList(e);
    if (!list) return node;

    ExpressionList* cur = list;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = node;
    return list;
}

void
parserutil_print_expression_list(ExpressionList* list) {
    printf("list: \n");
    while (list) {
        Term* t = list->expr->term;
        if (t == NULL) {
            printf("bad\n");
        }
        int i  = t->value.int_val;
        printf("(val: %d) ", i);
        list = list->next;
    }
    printf("\n");
}

// -----------------------------------------------------------------------------
// Subroutine Calls
// -----------------------------------------------------------------------------

SubroutineCall*
parserutil_create_direct_call(char* subroutineName, ExpressionList* exprList) {
    SubroutineCall* subcall = safe_malloc(sizeof(SubroutineCall));
    subcall->subroutineName = safe_strdup(subroutineName);
    subcall->exprList = exprList;
    subcall->caller = NULL;
    return subcall;
}

SubroutineCall*
parserutil_create_method_call(char* caller, char* subroutineName, ExpressionList* exprList) {
    SubroutineCall* subcall = safe_malloc(sizeof(SubroutineCall));
    subcall->subroutineName = safe_strdup(subroutineName);
    subcall->exprList = exprList;
    subcall->caller = safe_strdup(caller);
    return subcall;
}

// -----------------------------------------------------------------------------
// Type Utilities
// -----------------------------------------------------------------------------

VarType*
parserutil_create_var_type(VarTypeKind kind, char* name) {
    check_if_name_is_valid_class(name);
    VarType* type = safe_malloc(sizeof(VarType));
    type->kind = kind;
    type->className = name;
    return type;
}

// -----------------------------------------------------------------------------
// Subroutine Utilities
// -----------------------------------------------------------------------------

Subroutine*
parserutil_create_subroutine(SubroutineType type, VarType* returnType, char* name) {
    Subroutine* r = safe_malloc(sizeof(Subroutine));
    r->type = type;
    r->returnType = common_copy_vartype(returnType);
    r->name = strdup(name);
    return r;
}

void
parserutil_insert_subroutine(Subroutine* subroutine) {
    if (classSymtab_lookup_routine(CLASS_SYMTAB, subroutine->name) != NULL) {
        printf("[Error]: %s:%d: Subroutine \"%s\" is already defined.\n", FULL_SRC_PATH, yylineno, subroutine->name);
        exit(1);
    }
    classSymtab_insert_routine(CLASS_SYMTAB, subroutine);
}

void
parser_util_free_subroutine(Subroutine* r) {
    free(r->name);
    free(r);
}

// -----------------------------------------------------------------------------
// Validation
// -----------------------------------------------------------------------------

void
parserutil_validate_class_name(char* className) {
    assert(gbl_context.currentSourceName);
    if (strcmp(className, gbl_context.currentSourceName) != 0) {
        printf("Error: Class name \"%s\" does not match file name.\n", className);
        exit(1);
    }
}

// -----------------------------------------------------------------------------
// Memory Management
// -----------------------------------------------------------------------------

void
parserutil_free_let_statement(char* varName, Expression* e) {
    free_expression(e);
    free(varName);
}

void
parserutil_free_if_expression(Expression* e) {
    free_expression(e);
}

void
parserutil_free_while_expression(Expression* e) {
    free_expression(e);
}

void
parserutil_free_do_statement(SubroutineCall* call) {
    free_subroutine_call(call);
}

void
parser_util_free_return_statement(Expression* e) {
    if (e) {
        free_expression(e);
    }
}

// -----------------------------------------------------------------------------
// Static Definitions
// -----------------------------------------------------------------------------

static void
check_class_var_redeclared(char* name) {
    if (classSymtab_lookup_variable(CLASS_SYMTAB, name) != NULL) {
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

static void
check_if_name_is_valid_class(char* name) {
    if (name && !common_is_class_name(name)) {
        printf("Error at line %d: \"%s\" is not a valid class name.\n", yylineno, name);
        exit(1);
    }
}

static void
free_expression(Expression* e) {
    Term* t = e->term;
    if (t != NULL) {
        free_term(t);
    }
    OpTerm* opTerm = e->rest;
    if (opTerm != NULL) {
        free_opterm(opTerm);
    }
    free(e);
}

static void
free_term(Term* t) {
    TermType type = t->type;
    switch(type) {
        case VAR_TERM: {
            free(t->value.var_val);
            break;
        }
        case GROUPED_TERM: {
            free_expression(t->value.expr_val);
            break;
        }
        case SUBROUTINE_TERM: {
            free_subroutine_call(t->value.call_val);
            break;
        }
        case UNARY_TERM: {
            free_unary_term(t->value.unary_val);
            break;
        }
        case STRING_TERM: {
            free(t->value.str_val);
            break;
        }
        default: {
            printf("[Error]: Uknown term type.\n");
            exit(1);
        }
    }
    free(t);
}

static void
free_opterm(OpTerm* opTerm) {
    OpTerm* cur = opTerm;
    while (cur) {
        if (cur->term) {
            free_term(cur->term);
        }
        OpTerm* next = cur->next;
        free(cur);
        cur = next;
    }
}

static void
free_subroutine_call(SubroutineCall* call) {
    free_expression_list(call->exprList);
    free(call->subroutineName);
    if (call->caller != NULL) {
        free(call->caller);
    }
    free(call);
}

static void
free_expression_list(ExpressionList* list) {
    ExpressionList* cur = list;
    while (cur) {
        if (cur->expr) {
            free_expression(cur->expr);
        }
        ExpressionList* next = cur->next;
        free(cur);
        cur = next;
    }
}

static void
free_unary_term(UnaryTerm* t) {
    free(t->term);
    free(t);
}
