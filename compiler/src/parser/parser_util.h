#ifndef PARSER_UTIL_H
#define PARSER_UTIL_H

void
parserutil_validate_class_name(char* className);

<<<<<<< HEAD
ClassVarDecList*
parserutil_create_class_var_list(char* name);

void
parserutil_append_class_var(ClassVarDecList* list, char* name);

void
parserutil_print_class_var_list(ClassVarDecList* list);

void
parserutil_free_class_var_list(ClassVarDecList* list);
=======
VarDecList*
parserutil_create_class_var_list(char* name);

void
parserutil_append_var(VarDecList* list, char* name);

void
parserutil_print_var_list(VarDecList* list);

void
parserutil_free_var_list(VarDecList* list);
>>>>>>> c0c37b5 (implement subroutine level symbol table)

VarType*
parserutil_create_var_type(VarTypeKind kind, char* name);

void
<<<<<<< HEAD
parserutil_insert_class_variables(ClassScopeType kind, VarType* type, ClassVarDecList* list);
=======
parserutil_insert_class_variables(ClassScopeType kind, VarType* type, VarDecList* list);

void
parserutil_insert_routine_variables(RoutineScopeType kind, VarType* type, VarDecList* list);
>>>>>>> c0c37b5 (implement subroutine level symbol table)

#endif