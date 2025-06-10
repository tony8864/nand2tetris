#include "codegen_util.h"
#include "common.h"

#include <string.h>
#include <stdarg.h>

#define emit(...)   tracked_emit(out, __VA_ARGS__)

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

void
generate_bootstrap_code(FILE* out) {
    emit("// bootstrap code\n");
    emit("@256\n");
    emit("D=A\n");
    emit("@0\n");
    emit("M=D\n\n");
    emit("@Sys.init\n");
    emit("0;JMP\n\n");
}