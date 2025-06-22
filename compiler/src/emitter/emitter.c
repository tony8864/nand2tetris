#include "routine_symbol_table.h"
#include "class_symbol_table.h"
#include "safe_util.h"
#include "emitter.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

typedef struct UnaryTerm {
    Term* term;
    UnaryOperationType op;
} UnaryTerm;

typedef struct Term {
    TermType type;
    union {
        int int_val;
        char* var_val;
        struct Expression* expr_val;
        struct UnaryTerm*  unary_val;
        struct SubroutineCall* call_val;
    } value;
} Term;

typedef struct OpTerm {
    Term* term;
    OperationType op;
    struct OpTerm* next;
} OpTerm;

typedef struct Expression {
    Term* term;
    OpTerm* rest;
} Expression;

typedef struct ExpressionList {
    Expression* expr;
    struct ExpressionList* next;
} ExpressionList;

typedef struct SubroutineCall {
    char* subroutineName;
    ExpressionList* exprList;
    char* caller; // NULL if the call is direct
} SubroutineCall;

extern int yylineno;

static unsigned if_label_count = 0;
static unsigned while_label_count = 0;

static void
emit(const char* fmt, ...);

static void
emit_subroutine_term(SubroutineCall* call);

static void
emit_expression_list(ExpressionList* list);

static void
emit_subroutine_call(SubroutineCall* call);

static VarType*
get_vartype_from_name(char* name);

static void
emit_unary_term(UnaryTerm* unary);

static void
emit_expression(Expression* e);

static void
emit_term(Term* term);

static  void
emit_opTermList(OpTerm* opTerm);

static void
emit_op(OperationType op);

static void
emit_unary_op(UnaryOperationType op);

static void
emit_variable(char* name, char* op);

static void
emit_routine_var(RoutineSymbolTableEntry* entry, char* op);

static void
emit_class_var(ClassSymbolTableEntry* entry, char* op);

static void
emit_var_access(char* op, char* kind, unsigned index);

static unsigned
get_expr_list_len(ExpressionList* list);

static unsigned
is_in_symbol_table(char* name);

static char*
generate_if_label();

static char*
generate_while_label();

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

void
emitter_free_let_statement(char* varName, Expression* e) {
    free_expression(e);
    free(varName);
}

void
emitter_free_if_expression(Expression* e) {
    free_expression(e);
}

void
emitter_free_while_expression(Expression* e) {
    free_expression(e);
}

void
emitter_free_do_statement(SubroutineCall* call) {
    free_subroutine_call(call);
}

void
emitter_generate_if_expression(Expression* e) {
    char* label = generate_if_label();
    emit_expression(e);
    emit("not\n");
    emit("if-goto %s\n", label);
    free(label);
}

void
emitter_generate_after_if_statements() {
    char* label1 = generate_if_label();
    if_label_count++;
    char* label2 = generate_if_label();
    emit("goto %s\n", label2);
    emit("label %s\n", label1);
    free(label1);
    free(label2);
}

void
emitter_generate_after_optionalElse() {
    char* label2 = generate_if_label();
    emit("label %s\n", label2);
    if_label_count++;
    free(label2);
}

void
emitter_generate_while_expression(Expression* e) {
    char* label1 = generate_while_label();
    while_label_count++;
    emit("label %s\n", label1);
    emit_expression(e);
    emit("not\n");
    char* label2 = generate_while_label();
    emit("if-goto %s\n", label2);
    free(label1);
    free(label2);
}

void
emitter_generate_after_while_statements() {
    while_label_count--;
    char* label1 = generate_while_label();
    while_label_count++;
    char* label2 = generate_while_label();
    while_label_count++;
    emit("goto %s\n", label1);
    emit("label %s\n", label2);
    free(label1);
    free(label2);
}

void
emitter_generate_let_statement(char* varName, Expression* e) {
    emit_expression(e);
    emit_variable(varName, "pop");
}

void
emitter_generate_return_statement(Expression* e) {
    if (e == NULL) {
        emit("push constant 0\n");
        emit("return\n");
    }
    else {
        emit_expression(e);
        emit("return\n");
    }
}

void
emitter_generate_do_statement(SubroutineCall* call) {
    emit_expression_list(call->exprList);
    emit_subroutine_call(call);
    emit("pop temp 0\n");
}

Term*
emitter_create_int_term(int int_val) {
    Term* t = safe_malloc(sizeof(Term));
    t->type = INT_TERM;
    t->value.int_val = int_val;
    return t;
}

Term*
emitter_create_var_term(char* name) {
    Term* t = safe_malloc(sizeof(Term));
    t->type = VAR_TERM;
    t->value.var_val = safe_strdup(name);
    return t;
}

Term*
emitter_create_grouped_term(Expression* e) {
    Term* t = safe_malloc(sizeof(Term));
    t->type = GROUPED_TERM;
    t->value.expr_val = e;
    return t;
}

OpTerm*
emitter_create_op_term(OperationType op, Term* term) {
    OpTerm* opterm = safe_malloc(sizeof(OpTerm));
    opterm->op = op;
    opterm->term = term;
    opterm->next = NULL;
    return opterm;
}

Term*
emitter_create_subroutine_term(SubroutineCall* call) {
    Term* t = safe_malloc(sizeof(Term));
    t->type = SUBROUTINE_TERM;
    t->value.call_val = call;
    return t;
}

Term*
emitter_create_unary_term(UnaryOperationType op, Term* term) {
    Term* t = safe_malloc(sizeof(Term));
    UnaryTerm* ut = safe_malloc(sizeof(UnaryTerm));
    ut->term = term;
    ut->op = op;
    t->value.unary_val = ut;
    t->type = UNARY_TERM;
    return t;
}

Expression*
emitter_create_expression(Term* term, OpTerm* opTerm) {
    Expression* e = safe_malloc(sizeof(Expression));
    e->term = term;
    e->rest = opTerm;
    return e;
}

ExpressionList*
emitter_create_expressionList(Expression* expr) {
    ExpressionList* list = safe_malloc(sizeof(ExpressionList));
    list->expr = expr;
    list->next = NULL;
    return list;
}

ExpressionList*
emiiter_append_expression(ExpressionList* list, Expression* e) {
    ExpressionList* node = emitter_create_expressionList(e);
    if (!list) return node;

    ExpressionList* cur = list;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = node;
    return list;
}

void
emitter_print_expression_list(ExpressionList* list) {
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

SubroutineCall*
emitter_create_direct_call(char* subroutineName, ExpressionList* exprList) {
    SubroutineCall* subcall = safe_malloc(sizeof(SubroutineCall));
    subcall->subroutineName = safe_strdup(subroutineName);
    subcall->exprList = exprList;
    subcall->caller = NULL;
    return subcall;
}

SubroutineCall*
emitter_create_method_call(char* caller, char* subroutineName, ExpressionList* exprList) {
    SubroutineCall* subcall = safe_malloc(sizeof(SubroutineCall));
    subcall->subroutineName = safe_strdup(subroutineName);
    subcall->exprList = exprList;
    subcall->caller = safe_strdup(caller);
    return subcall;
}

OpTerm*
emitter_append_op_term(OpTerm* list, OpTerm* node) {
    if (!list) return node;

    OpTerm* cur = list;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = node;
    return list;
}

static void
emit(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(VM_FILE, fmt, args);
    va_end(args);
}

static void
emit_term(Term* term) {
    switch(term->type) {
        case INT_TERM: {
            emit("push constant %d\n", term->value.int_val);
            break;
        }
        case VAR_TERM: {
            emit_variable(term->value.var_val, "push");
            break;
        }
        case GROUPED_TERM: {
            emit_expression(term->value.expr_val);
            break;
        }
        case UNARY_TERM: {
            emit_unary_term(term->value.unary_val);
            break;
        }
        case SUBROUTINE_TERM: {
            emit_subroutine_term(term->value.call_val);
            break;
        }
    }
}

static void
emit_subroutine_term(SubroutineCall* call) {
    emit_expression_list(call->exprList);
    emit_subroutine_call(call);
}

static void
emit_expression_list(ExpressionList* list) {
    while (list) {
        emit_expression(list->expr);
        list = list->next;
    }
}

static void
emit_subroutine_call(SubroutineCall* call) {
    unsigned argsCount = get_expr_list_len(call->exprList);
    if (is_in_symbol_table(call->caller)) {
        VarType* type = get_vartype_from_name(call->caller);
        char* className = common_get_classname_from_type(type);
        emit("call %s.%s %u\n", className, call->subroutineName, argsCount);
    }
    else {
        emit("call %s.%s %u\n", call->caller, call->subroutineName, argsCount);
    }
}

static void
emit_unary_term(UnaryTerm* unary) {
    emit_term(unary->term);
    emit_unary_op(unary->op);
}

static void
emit_unary_op(UnaryOperationType op) {
    switch(op) {
        case UNARY_MINUS: {
            emit("neg\n");
            break;
        }
        case UNARY_NEG: {
            emit("not\n");
            break;
        }
        default: printf("Error: Unrecognized unary op.\n"); exit(1);
    }
}

static void
emit_expression(Expression* e) {
    Term* term = e->term;
    OpTerm* opTerm = e->rest;
    emit_term(term);
    emit_opTermList(opTerm);
}

static  void
emit_opTermList(OpTerm* opTerm) {
    while (opTerm) {
        emit_term(opTerm->term);
        emit_op(opTerm->op);
        opTerm = opTerm->next;
    }
}

static void
emit_op(OperationType op) {
    switch(op) {
        case PLUS_OP:    emit("add\n"); break;
        case MINUS_OP:   emit("sub\n"); break;
        case AND_OP:     emit("and\n"); break;
        case OR_OP:      emit("or\n"); break;
        case LESS_OP:    emit("lt\n"); break;
        case GREATER_OP: emit("gt\n"); break;
        case EQUAL_OP:   emit("eq\n"); break;
        default: printf("Error: Unexpected operation type.\n"); exit(1);
    }
}

static void
emit_variable(char* name, char* op) {
    RoutineSymbolTableEntry* routineEntry = routineSymtab_lookup(ROUTINE_SYMTAB, name);
    if (routineEntry != NULL) {
        emit_routine_var(routineEntry, op);
        return;
    }

    ClassSymbolTableEntry* classEntry = classSymtab_lookup(CLASS_SYMTAB, name);
    if (classEntry != NULL) {
        emit_class_var(classEntry, op);
        return;
    }

    printf("Error: Undeclared variable \"%s\" at line %d\n", name, yylineno);
    exit(1);
}

static void
emit_routine_var(RoutineSymbolTableEntry* entry, char* op) {
    unsigned index = routineSymtab_get_entry_index(entry);
    char*    kind  = routineSymtab_get_str_kind(entry);
    emit_var_access(op, kind, index);
}

static void
emit_class_var(ClassSymbolTableEntry* entry, char* op) {
    unsigned index = classSymtab_get_entry_index(entry);
    char*    kind  = classSymtab_get_str_kind(entry);
    emit_var_access(op, kind, index);
}

static void
emit_var_access(char* op, char* kind, unsigned index) {
    emit("%s %s %u\n", op, kind, index);
}

static unsigned
get_expr_list_len(ExpressionList* list) {
    unsigned i = 0;
    while (list) {
        list = list->next;
        i++;
    }
    return i;
}

static unsigned
is_in_symbol_table(char* name) {
    return (classSymtab_lookup(CLASS_SYMTAB, name) != NULL ||
    routineSymtab_lookup(ROUTINE_SYMTAB, name) != NULL);
}

static VarType*
get_vartype_from_name(char* name) {
    ClassSymbolTableEntry* classEntry = classSymtab_lookup(CLASS_SYMTAB, name);
    if (classEntry != NULL) {
        return classSymtab_get_vartype(classEntry);
    }

    RoutineSymbolTableEntry* routineEntry = routineSymtab_lookup(ROUTINE_SYMTAB, name);
    if (routineEntry != NULL) {
        return routineSymtab_get_vartype(routineEntry);
    }

    printf("Error: Could not retrieve variable type from symbol tables.\n");
    exit(1);
}

static char*
generate_if_label() {
    char* label = malloc(32);
    snprintf(label, 32, "IF_LABEL_%d", if_label_count);
    return label;
}

static char*
generate_while_label() {
    char* label = malloc(32);
    snprintf(label, 32, "WHILE_LABEL_%d", while_label_count);
    return label;
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
            printf("calling free routine\n");
            free_subroutine_call(t->value.call_val);
            break;
        }
        case UNARY_TERM: {
            free_unary_term(t->value.unary_val);
            break;
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