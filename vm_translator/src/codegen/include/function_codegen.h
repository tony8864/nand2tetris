#ifndef FUNCTION_CODEGEN_H
#define FUNCTION_CODEGEN_H

#include <stdio.h>

void
generate_function_operation(FILE* out, char* func, unsigned locals);

void
generate_call_operation(FILE* out, char* func, unsigned args);

void
generate_return_operation(FILE* out);

#endif