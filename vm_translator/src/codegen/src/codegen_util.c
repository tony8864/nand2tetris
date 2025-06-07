#include "codegen_util.h"
#include "common.h"

#include <string.h>
#include <stdarg.h>

void
tracked_emit(FILE* out, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    if (strncmp(fmt, "//", 2) != 0 && strcmp(fmt, "\n") != 0) {
        vm_context.instruction_count++;
    }

    vfprintf(out, fmt, args);
    va_end(args);
}