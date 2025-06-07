#ifndef VM_PARSER_UTIL_H
#define VM_PARSER_UTIL_H

#include "common.h"

void
vmparserUtil_open_out_file(char* filename);

void
vmparserUtil_combine_asm_files(char* filename);

void
vmparserUtil_handleMemoryOperation(MemOp_T op, Segment_T seg, int index);

void
vmparserUtil_handleComputeOperation(ComputeOp_T cmd);

void
vmparserUtil_handleLabelOperation(LabelOp_T cmd, char* label);

void
vmparserUtil_cleanup();

#endif