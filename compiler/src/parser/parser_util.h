#ifndef PARSER_UTIL_H
#define PARSER_UTIL_H

void
parserutil_validate_class_name(char* className);

VarDecList*
parserutil_create_class_var_list(char* name);

void
parserutil_append_var(VarDecList* list, char* name);

void
parserutil_print_var_list(VarDecList* list);

void
parserutil_free_var_list(VarDecList* list);

VarType*
parserutil_create_var_type(VarTypeKind kind, char* name);

void
parserutil_insert_class_variables(ClassScopeType kind, VarType* type, VarDecList* list);

void
parserutil_insert_routine_variables(RoutineScopeType kind, VarType* type, VarDecList* list);

#endif