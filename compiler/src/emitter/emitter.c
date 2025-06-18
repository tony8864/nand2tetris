#include "emitter.h"

#include <stdio.h>
#include <stdarg.h>

void
emit(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(VM_FILE, fmt, args);
    va_end(args);
}

void
emit_term(Term* term) {
    if (term->type == INT_TERM) {
        emit("push constant %d\n", term->value.int_val);
    }
}

void
emit_op(OperationType op) {
    if (op == PLUS_OP) {
        emit("add\n");
    }
    else if (op == MINUS_OP) {
        emit("sub\n");
    }
}