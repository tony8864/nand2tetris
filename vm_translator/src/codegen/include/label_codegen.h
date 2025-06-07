#ifndef LABEL_CODEGEN_H
#define LABEL_CODEGEN_H

#include <stdio.h>

#include "common.h"

void
generate_label_operation(FILE* out, LabelOp_T op, char* label);

#endif