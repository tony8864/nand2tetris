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
vmparserUtil_handleMemoryOperation(MemOp_T op, Segment_T seg, int index);

void
vmparserUtil_handleComputeOperation(ComputeOp_T cmd);

void
vmparserUtil_handleLabelOperation(LabelOp_T cmd, char* label);

void
vmparserUtil_handleFunctionOperation(char* func, int args);

void
vmparserUtil_handleCallOperation(char* func, int locals);

void
vmparserUtil_handleReturnOperation();

void
vmparserUtil_cleanup();

#endif