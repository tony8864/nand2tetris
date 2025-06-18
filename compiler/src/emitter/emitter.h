#ifndef EMITTER_H
#define EMITTER_H

#include "common.h"

void
emit(const char* fmt, ...);

void
emit_term(Term* term);

void
emit_op(OperationType op);

#endif