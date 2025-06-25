#ifndef EMITTER_H
#define EMITTER_H

#include "common.h"

// -----------------------------------------------------------------------------
// VM Code Generation for Statements
// -----------------------------------------------------------------------------
void
emitter_generate_if_expression(Expression* e);

void
emitter_generate_if_without_else();

void
emitter_generate_if_with_else();

void
emitter_generate_after_else();

void
emitter_generate_while_expression(Expression* e);

void
emitter_generate_after_while_statements();

void
emitter_generate_let_statement(char* varName, Expression* e);

void
emitter_generate_array_let_statement(char* varName, Expression* expr1, Expression* expr2);

void
emitter_generate_return_statement(Expression* e);

void
emitter_generate_do_statement(SubroutineCall* call);

void
emitter_generate_subroutine();

#endif
