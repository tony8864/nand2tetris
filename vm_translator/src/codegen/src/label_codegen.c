#include "codegen_util.h"
#include "label_codegen.h"

#define emit(...)           tracked_emit(out, __VA_ARGS__)

static void codegen_label   (FILE* out, char* label);
static void codegen_goto    (FILE* out, char* label);
static void codegen_ifgoto  (FILE* out, char* label);

typedef void (*label_codegen_func_t)(FILE* out, char* label);

label_codegen_func_t labelCodegenFuncs[] = {
    codegen_label,
    codegen_goto,
    codegen_ifgoto
};

void
generate_label_operation(FILE* out, LabelOp_T op, char* label) {
    (*labelCodegenFuncs[op])(out, label);
}

static void codegen_label(FILE* out, char* label) {
    emit("(%s)\n\n", label);
}

static void codegen_goto(FILE* out, char* label) {
    emit("// begin goto label\n\n");

    emit("@%s\n", label);
    emit("0;JMP\n\n");

    emit("// end goto label\n\n");
}

static void codegen_ifgoto(FILE* out, char* label) {
    emit("// begin if-goto label\n\n");

    emit("// D = mem[top]\n");
    emit("@SP\n");
    emit("AM=M-1\n");
    emit("D=M\n\n");

    emit("// if D != 0 goto label\n");
    emit("@%s\n", label);
    emit("D;JNE\n\n");
    
    emit("// end if-goto label\n\n");
}