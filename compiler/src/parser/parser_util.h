#ifndef PARSER_UTIL_H
#define PARSER_UTIL_H

// -----------------------------------------------------------------------------
// Param List Utilities
// -----------------------------------------------------------------------------

ParamList*
parserutil_create_param_list();

Param*
parserutil_create_param(VarType* type, char* name);

void
parserutil_append_param(ParamList* list, Param* param);

void
parserutil_insert_parameters(ParamList* list);

void
parserutil_free_param_list(ParamList* list);

void
parserutil_print_param_list(ParamList* list);

// -----------------------------------------------------------------------------
// Variable Declaration Utilities
// -----------------------------------------------------------------------------

VarDecList*
parserutil_create_class_var_list(char* name);

void
parserutil_append_var(VarDecList* list, char* name);

void
parserutil_insert_class_variables(ClassScopeType kind, VarType* type, VarDecList* list);

void
parserutil_insert_routine_variables(RoutineScopeType kind, VarType* type, VarDecList* list);

void
parserutil_free_var_list(VarDecList* list);

void
parserutil_print_var_list(VarDecList* list);

// -----------------------------------------------------------------------------
// Term Utilities
// -----------------------------------------------------------------------------

Term*
parserutil_create_int_term(int int_val);

Term*
parserutil_create_var_term(char* name);

Term*
parserutil_create_grouped_term(Expression* e);

Term*
parserutil_create_subroutine_term(SubroutineCall* call);

Term*
parserutil_create_unary_term(UnaryOperationType op, Term* term);

Term*
parserutil_create_keyword_const_term(KeywordConstType type);

// -----------------------------------------------------------------------------
// Operators and Expressions
// -----------------------------------------------------------------------------

OpTerm*
parserutil_create_op_term(OperationType op, Term* term);

OpTerm*
parserutil_append_op_term(OpTerm* list, OpTerm* node);

Expression*
parserutil_create_expression(Term* term, OpTerm* opTerm);

// -----------------------------------------------------------------------------
// Expression Lists
// -----------------------------------------------------------------------------

ExpressionList*
parserutil_create_expressionList(Expression* expr);

ExpressionList*
parserutil_append_expression(ExpressionList* list, Expression* e);

void
parserutil_print_expression_list(ExpressionList* list);

// -----------------------------------------------------------------------------
// Subroutine Calls
// -----------------------------------------------------------------------------

SubroutineCall*
parserutil_create_direct_call(char* subroutineName, ExpressionList* exprList);

SubroutineCall*
parserutil_create_method_call(char* caller, char* subroutineName, ExpressionList* exprList);


// -----------------------------------------------------------------------------
// Type Utilities
// -----------------------------------------------------------------------------

VarType*
parserutil_create_var_type(VarTypeKind kind, char* name);

// -----------------------------------------------------------------------------
// Subroutine Utilities
// -----------------------------------------------------------------------------

Subroutine*
parserutil_create_subroutine(SubroutineType type, VarType* returnType, char* name);

void
parserutil_insert_subroutine(Subroutine* subroutine);

void
parser_util_free_subroutine(Subroutine* r);

// -----------------------------------------------------------------------------
// Validation
// -----------------------------------------------------------------------------

void
parserutil_validate_class_name(char* className);

// -----------------------------------------------------------------------------
// Memory Management
// -----------------------------------------------------------------------------

void
parserutil_free_let_statement(char* varName, Expression* e);

void
parserutil_free_if_expression(Expression* e);

void
parserutil_free_while_expression(Expression* e);

void
parserutil_free_do_statement(SubroutineCall* call);

void
parser_util_free_return_statement(Expression* e);

#endif
