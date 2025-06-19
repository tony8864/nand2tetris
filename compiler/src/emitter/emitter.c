#include "routine_symbol_table.h"
#include "class_symbol_table.h"
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

extern int yylineno;

static void
emit(const char* fmt, ...);

static void
emit_expression(Expression* e);

static void
emit_term(Term* term);

static  void
emit_opTermList(OpTerm* opTerm);

static void
emit_op(OperationType op);

static void
emit_variable(char* name, char* op);

static void
emit_routine_var(RoutineSymbolTableEntry* entry, char* op);

static void
emit_class_var(ClassSymbolTableEntry* entry);

static void
emit_var_access(char* op, char* kind, unsigned index);

void
emitter_generate_let_statement(char* varName, Expression* e) {
    emit_expression(e);
    emit_variable(varName, "pop");
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
        case VAR_TERM: {
            emit_variable(term->value.var_val, "push");
            break;
        }
        case GROUPED_TERM: {
            emit_expression(term->value.expr_val);
        }
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
        case PLUS_OP:   emit("add\n"); break;
        case MINUS_OP:  emit("sub\n"); break;
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
        emit_class_var(classEntry);
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
emit_class_var(ClassSymbolTableEntry* entry) {
    
}

static void
emit_var_access(char* op, char* kind, unsigned index) {
    emit("%s %s %u\n", op, kind, index);
}
