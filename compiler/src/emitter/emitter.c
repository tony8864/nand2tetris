#include "routine_symbol_table.h"
#include "class_symbol_table.h"
#include "safe_util.h"
#include "emitter.h"
#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

extern int yylineno;

static unsigned if_label_count = 0;
static unsigned while_label_count = 0;

#define IS_CALL_DIRECT(call)  ((call)->caller == NULL)

// ───────────── Emit Core ─────────────
static void emit(const char* fmt, ...);

// ───── Emit: Expressions & Terms ─────
static void emit_expression(Expression* e);
static void emit_term(Term* term);
static void emit_unary_term(UnaryTerm* unary);
static void emit_expression_list(ExpressionList* list);
static void emit_opTermList(OpTerm* opTerm);
static void emit_keywordconst_term(KeywordConstType type);

// ──── Emit: Subroutines & Calls ─────
static void emit_subroutine_call(SubroutineCall* call);
static void emit_direct_call(SubroutineCall* call);
static void emit_indirect_call(SubroutineCall* call);
static void emit_method_call(SubroutineCall* call);
static void emit_function_call(SubroutineCall* call);
static void is_method_in_current_class(char* name);
static void emit_subroutine_term(SubroutineCall* call);

// ───── Emit: Variables & Symbols ─────
static void emit_variable(char* name, char* op);
static void emit_routine_var(RoutineSymbolTableEntry* entry, char* op);
static void emit_class_var(ClassSymbolTableEntry* entry, char* op);
static void emit_var_access(char* op, char* kind, unsigned index);

// ───── Emit: Operators ─────
static void emit_op(OperationType op);
static void emit_unary_op(UnaryOperationType op);

// ───── Symbol Table / Type Helpers ─────
static VarType* get_vartype_from_name(char* name);
static unsigned is_in_symbol_table(char* name);

// ───── Utility Generators ─────
static unsigned get_expr_list_len(ExpressionList* list);
static char* generate_if_label();
static char* generate_while_label();
static void generate_constructor();
static void generate_function();
static void generate_method();
static char* get_constructor_full_name();

// -----------------------------------------------------------------------------
// VM Code Generation for Statements
// -----------------------------------------------------------------------------

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
    emit_subroutine_call(call);
    emit("pop temp 0\n");
}

void
emitter_generate_subroutine() {
    switch(CURRENT_SUBROUTINE->type) {
        case CONSTRUCTOR_TYPE: {
            generate_constructor();
            break;
        }
        case FUNCTION_TYPE: {
            generate_function();
            break;
        }
        case METHOD_TYPE: {
            generate_method();
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// Static Definitions
// -----------------------------------------------------------------------------

// ───────────── Emit Core ─────────────
static void
emit(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(VM_FILE, fmt, args);
    va_end(args);
}

// ───── Emit: Expressions & Terms ─────
static void
emit_expression(Expression* e) {
    Term* term = e->term;
    OpTerm* opTerm = e->rest;
    emit_term(term);
    emit_opTermList(opTerm);
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
        case KEYWORDCONST_TERM: {
            emit_keywordconst_term(term->value.keywordconst_val);
        }
    }
}

static void
emit_unary_term(UnaryTerm* unary) {
    emit_term(unary->term);
    emit_unary_op(unary->op);
}

static void
emit_expression_list(ExpressionList* list) {
    while (list) {
        emit_expression(list->expr);
        list = list->next;
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
emit_keywordconst_term(KeywordConstType type) {
    switch(type) {
        case KEYWORD_TRUE: {
            emit("push constant 0\n");
            break;
        }
        case KEYWORD_FALSE: {
            emit("push constant 0\n");
            emit("not\n");
            break;
        }
        case KEYWORD_THIS: {
            emit("push pointer 0\n");
            break;
        }
        case KEYWORD_NULL: {
            emit("push constant 0\n");
            break;
        }
    }
}

// ──── Emit: Subroutines & Calls ─────
static void
emit_subroutine_call(SubroutineCall* call) {
    if (IS_CALL_DIRECT(call)) {
        emit_direct_call(call);
    }
    else {
        emit_indirect_call(call);
    }
}

static void
emit_direct_call(SubroutineCall* call) {
    unsigned args = 1 + get_expr_list_len(call->exprList);
    emit("push pointer 0\n");
    emit_expression_list(call->exprList);
    emit("call %s.%s %d\n", SRC_NAME, call->subroutineName, args);
}

static void
emit_indirect_call(SubroutineCall* call) {
    unsigned is_variable = is_in_symbol_table(call->caller);
    unsigned is_class = common_is_class_name(call->caller);

    if (!is_variable && !is_class) {
        printf("[Error]: %s:%d: Unrecognized symbol \"%s\".\n", FULL_SRC_PATH, yylineno, call->caller);
        exit(1);
    }

    if (is_variable) {
        emit_method_call(call);
    }
    else {
        emit_function_call(call);
    }
}

static void
emit_method_call(SubroutineCall* call) {
    VarType* originalType = get_vartype_from_name(call->caller);
    VarType* varType = common_copy_vartype(originalType);
    char* className = common_get_classname_from_type(varType);
    unsigned args = 1 + get_expr_list_len(call->exprList);

    emit_variable(call->caller, "push");
    emit_expression_list(call->exprList);
    emit("call %s.%s %d\n", className, call->subroutineName, args);

    common_free_vartype(varType);
}

static void emit_function_call(SubroutineCall* call) {
    unsigned args = get_expr_list_len(call->exprList);

    emit_expression_list(call->exprList);
    emit("call %s.%s %d\n", call->caller, call->subroutineName, args);
}

static void
is_method_in_current_class(char* name) {
    SubroutineEntry* entry = classSymtab_lookup_routine(CLASS_SYMTAB, name);
    if (!entry) {
        printf("[Error]: %s:%d: Method \"%s\" does not exist in the current class.\n", FULL_SRC_PATH, yylineno, name);
        exit(1);
    }

    if (classSymtab_get_subroutine_type(entry) != METHOD_TYPE) {
        printf("[Error]: %s:%d: Subroutine \"%s\" called as method.\n", FULL_SRC_PATH, yylineno, name);
        exit(1);
    }
}

static void
emit_subroutine_term(SubroutineCall* call) {
    emit_subroutine_call(call);
}

// ───── Emit: Variables & Symbols ─────
static void
emit_variable(char* name, char* op) {
    RoutineSymbolTableEntry* routineEntry = routineSymtab_lookup(ROUTINE_SYMTAB, name);
    if (routineEntry != NULL) {
        emit_routine_var(routineEntry, op);
        return;
    }

    ClassSymbolTableEntry* classEntry = classSymtab_lookup_variable(CLASS_SYMTAB, name);
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

// ───── Emit: Operators ─────
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

// ───── Symbol Table / Type Helpers ─────
static VarType*
get_vartype_from_name(char* name) {
    ClassSymbolTableEntry* classEntry = classSymtab_lookup_variable(CLASS_SYMTAB, name);
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

static unsigned
is_in_symbol_table(char* name) {
    ClassSymbolTableEntry* classEntry = classSymtab_lookup_variable(CLASS_SYMTAB, name);
    RoutineSymbolTableEntry* routineEntry = routineSymtab_lookup(ROUTINE_SYMTAB, name);
    return (classEntry != NULL || routineEntry != NULL);
}

// ───── Utility Generators ─────
static unsigned
get_expr_list_len(ExpressionList* list) {
    unsigned i = 0;
    while (list) {
        list = list->next;
        i++;
    }
    return i;
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
generate_constructor() {
    unsigned locals = routineSymtab_get_locals_count(ROUTINE_SYMTAB);
    unsigned fields = classSymtab_get_fields_count(CLASS_SYMTAB);
    char* constructor_name = get_constructor_full_name();
    
    emit("function %s %d\n", constructor_name, locals);
    emit("push constant %d\n", fields);
    emit("call Memory.alloc 1\n");
    emit("pop pointer 0\n");

    free(constructor_name);
}

static void
generate_function() {
    unsigned locals = routineSymtab_get_locals_count(ROUTINE_SYMTAB);
    emit("function %s %d\n", CURRENT_SUBROUTINE->name, locals);
}

static void
generate_method() {
    unsigned locals = routineSymtab_get_locals_count(ROUTINE_SYMTAB);

    emit("function %s %d\n", CURRENT_SUBROUTINE->name, locals);
    emit("push argument 0\n");
    emit("pop pointer 0\n");
}

static char*
get_constructor_full_name() {
    char* subroutine_name = CURRENT_SUBROUTINE->name;
    char* src_name = SRC_NAME;

    size_t len = strlen(subroutine_name) + strlen(src_name) + 1 + 1; // +1 for '.' +1 for'\0'
    char* full_name = safe_malloc(sizeof(char) * len);

    snprintf(full_name, len, "%s.%s", src_name, subroutine_name);
    return full_name;
}