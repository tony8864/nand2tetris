#ifndef VM_PARSER_UTIL_H
#define VM_PARSER_UTIL_H

#include "common.h"

void
vmparserUtil_open_out_file(char* filename);

void
vmparserUtil_combine_asm_files();

void
vmparserUtil_append_bootstrap_code();

void
vmparserUtil_cleanup();

#endif