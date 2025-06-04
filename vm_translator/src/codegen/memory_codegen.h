#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>

#include "common.h"

void
generate_memory_operation(FILE* out, MemOp_T op,  Segment_T seg, int index);

#endif