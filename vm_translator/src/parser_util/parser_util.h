#ifndef VM_PARSER_UTIL_H
#define VM_PARSER_UTIL_H

#include "common.h"

extern char* asm_filename;

void
vmparserUtil_openOutfile(char* filename);

void
vmparserUtil_handleMemoryOperation(MemOp_T op, Segment_T seg, int index);

void
vmparserUtil_handleComputeOperation(ComputeCommand_T cmd);

void
vmparserUtil_cleanup();

#endif