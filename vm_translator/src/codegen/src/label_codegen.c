#include "codegen_util.h"
#include "label_codegen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define emit(...)           tracked_emit(out, __VA_ARGS__)

static void codegen_label   (FILE* out, char* label);
static void codegen_goto    (FILE* out, char* label);
static void codegen_ifgoto  (FILE* out, char* label);

static char*
create_unique_label(char* label);

static char*
create_unique_label_func_absent(char* label);

static char*
create_unique_label_func_present(char* label);

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
    char* unique_label =  create_unique_label(label);
    emit("(%s)\n\n", unique_label);
    free(unique_label);
}

static void codegen_goto(FILE* out, char* label) {
    char* unique_label =  create_unique_label(label);
    emit("// begin goto label\n\n");

    emit("@%s\n", unique_label);
    emit("0;JMP\n\n");

    emit("// end goto label\n\n");
    free(unique_label);
}

static void codegen_ifgoto(FILE* out, char* label) {
    char* unique_label =  create_unique_label(label);
    emit("// begin if-goto label\n\n");

    emit("// D = mem[top]\n");
    emit("@SP\n");
    emit("AM=M-1\n");
    emit("D=M\n\n");

    emit("// if D != 0 goto label\n");
    emit("@%s\n", unique_label);
    emit("D;JNE\n\n");
    
    emit("// end if-goto label\n\n");
    free(unique_label);
}

static char*
create_unique_label(char* label) {
    char* unique_label;
    if (vm_context.current_function_name != NULL) {
        unique_label = create_unique_label_func_present(label);
    }
    else {
        unique_label = create_unique_label_func_absent(label);
    }

    return unique_label;
}

static char*
create_unique_label_func_absent(char* label) {
    char* file = vm_context.input_filename;

    size_t  size    = strlen(file) + strlen(label) + 1 + 1; //'.' and '\0' chars
    char*   unique  = malloc(sizeof(char) * size);
    if (!unique) {
        perror("malloc");
        exit(1);
    }

    sprintf(unique, "%s.%s", file, label);
    return unique;
}

static char*
create_unique_label_func_present(char* label) {
    char* file = vm_context.input_filename;
    char* func = vm_context.current_function_name;

    size_t  size    = strlen(file) + strlen(func) + strlen(label) + 1 + 1 + 1; // '.' and '$' and '\0' chars
    char*   unique  = malloc(sizeof(char) * size);
    if (!unique) {
        perror("malloc");
        exit(1);
    }

    sprintf(unique, "%s.%s$%s", file, func, label);

    return unique;
}