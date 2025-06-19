#ifndef EMITTER_H
#define EMITTER_H

#include "common.h"

void
emitter_generate_let_statement(char* varName, Expression* e);

Term*
emitter_create_int_term(int int_val);

Term*
emitter_create_var_term(char* name);

Term*
emitter_create_grouped_term(Expression* e);

OpTerm*
emitter_create_op_term(OperationType op, Term* term);

Term*
emitter_create_unary_term(UnaryOperationType op, Term* term);

Expression*
emitter_create_expression(Term* term, OpTerm* opTerm);

OpTerm*
emitter_append_op_term(OpTerm* list, OpTerm* node);

#endif