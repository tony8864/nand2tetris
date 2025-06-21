#ifndef EMITTER_H
#define EMITTER_H

#include "common.h"

void
emitter_generate_if_expression(Expression* e);

void
emitter_generate_after_if_statements();

void
emitter_generate_after_optionalElse();

void
emitter_generate_let_statement(char* varName, Expression* e);

void
emitter_generate_return_statement(Expression* e);

void
emitter_generate_do_statement(SubroutineCall* call);

Term*
emitter_create_int_term(int int_val);

Term*
emitter_create_var_term(char* name);

Term*
emitter_create_grouped_term(Expression* e);

OpTerm*
emitter_create_op_term(OperationType op, Term* term);

OpTerm*
emitter_append_op_term(OpTerm* list, OpTerm* node);

Expression*
emitter_create_expression(Term* term, OpTerm* opTerm);

ExpressionList*
emitter_create_expressionList(Expression* expr);

ExpressionList*
emiiter_append_expression(ExpressionList* list, Expression* e);

void
emitter_print_expression_list(ExpressionList* list);

SubroutineCall*
emitter_create_direct_call(char* subroutineName, ExpressionList* exprList);

SubroutineCall*
emitter_create_method_call(char* caller, char* subroutineName, ExpressionList* exprList);

Term*
emitter_create_subroutine_term(SubroutineCall* call);

Term*
emitter_create_unary_term(UnaryOperationType op, Term* term);

#endif