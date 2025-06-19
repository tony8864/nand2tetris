#include "safe_util.h"
#include "emitter.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

typedef struct Term {
    TermType type;
    union {
        int int_val;
        char* var_val;
        struct Expression* expr_val;
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

static void
emit(const char* fmt, ...);

static void
emit_term(Term* term);

static  void
emit_opTermList(OpTerm* opTerm);

static void
emit_op(OperationType op);

void
emit_expression(Expression* e) {
    Term* term = e->term;
    OpTerm* opTerm = e->rest;
    emit_term(term);
    emit_opTermList(opTerm);
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
    return opterm;
}

Expression*
emitter_create_expression(Term* term, OpTerm* opTerm) {
    Expression* e = safe_malloc(sizeof(Expression));
    e->term = term;
    e->rest = opTerm;
    return e;
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
        case GROUPED_TERM: {
            emit_expression(term->value.expr_val);
        }
    }
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
        case PLUS_OP:   emit("add\n"); break;
        case MINUS_OP:  emit("sub\n"); break;
        default: printf("Error: Unexpected operation type.\n"); exit(1);
    }
}