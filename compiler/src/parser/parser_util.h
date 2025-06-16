#ifndef PARSER_UTIL_H
#define PARSER_UTIL_H

void
parserutil_validate_class_name(char* className);

ClassVarDecList*
parserutil_create_class_var_list(char* name);

void
parserutil_append_class_var(ClassVarDecList* list, char* name);

void
parserutil_print_class_var_list(ClassVarDecList* list);

void
parserutil_free_class_var_list(ClassVarDecList* list);

VarType*
parserutil_create_var_type(VarTypeKind kind, char* name);

void
parserutil_insert_class_variables(ClassScopeType kind, VarType* type, ClassVarDecList* list);

#endif