#include "memory_codegen.h"
#include "parser_util.h"

#include <stdlib.h>
#include <string.h>

extern int yylineno;

// Index formula: [0..7] = pop funcs, [8..15] = push funcs
#define FUNC_INDEX(op, seg)     ((op) * MEMORY_SEGMENT_COUNT + (seg))
#define MEMORY_SEGMENT_COUNT    8

#define emit(...) fprintf(out, __VA_ARGS__)
#define emit_increment_sp()                                    \
                                emit("// SP++\n");             \
                                emit("@SP\n");                 \
                                emit("M=M+1\n\n")

#define emit_push_d_to_stack()                                 \
                                emit("// mem[SP] = D\n");      \
                                emit("@SP\n");                 \
                                emit("A=M\n");                 \
                                emit("M=D\n\n")



static void codegen_pop_local   (FILE* out, int index);
static void codegen_pop_argument(FILE* out, int index);
static void codegen_pop_this    (FILE* out, int index);
static void codegen_pop_that    (FILE* out, int index);
static void codegen_pop_pointer (FILE* out, int index);
static void codegen_pop_temp    (FILE* out, int index);
static void codegen_pop_constant(FILE* out, int index);
static void codegen_pop_static  (FILE* out, int index);

static void codegen_push_local   (FILE* out, int index);
static void codegen_push_argument(FILE* out, int index);
static void codegen_push_this    (FILE* out, int index);
static void codegen_push_that    (FILE* out, int index);
static void codegen_push_pointer (FILE* out, int index);
static void codegen_push_temp    (FILE* out, int index);
static void codegen_push_constant(FILE* out, int index);
static void codegen_push_static  (FILE* out, int index);

static void
writePushSegment(FILE* out, char* segment, int index);

static void
writePushPointer(FILE* out, char* segment);

static char*
make_combined_name(char* base, int index);

static char*
get_prefix_before_dot(char* input);

static char*
get_static_variable_name(int index);

typedef void (*mem_codegen_func_t)(FILE* out, int index);

mem_codegen_func_t codegenFuncs[] = {
    codegen_pop_local,
    codegen_pop_argument,
    codegen_pop_this,
    codegen_pop_that,
    codegen_pop_pointer,
    codegen_pop_temp,
    codegen_pop_constant,
    codegen_pop_static,
    codegen_push_local,
    codegen_push_argument,
    codegen_push_this,
    codegen_push_that,
    codegen_push_pointer,
    codegen_push_temp,
    codegen_push_constant,
    codegen_push_static
};

void
generate_memory_operation(FILE* out, MemOp_T op,  Segment_T seg, int index) {
    (*codegenFuncs[FUNC_INDEX(op, seg)])(out, index);
}

static void codegen_pop_local(FILE* out, int index) {

}

static void codegen_pop_argument(FILE* out, int index) {

}

static void codegen_pop_this(FILE* out, int index) {

}

static void codegen_pop_that(FILE* out, int index) {

}

static void codegen_pop_pointer(FILE* out, int index) {

}

static void codegen_pop_temp(FILE* out, int index) {

}

static void codegen_pop_constant(FILE* out, int index) {

}

static void codegen_pop_static(FILE* out, int index) {

}

static void codegen_push_local(FILE* out, int index) {
    writePushSegment(out, "LCL", index);
}

static void codegen_push_argument(FILE* out, int index) {
    writePushSegment(out, "ARG", index);
}

static void codegen_push_this(FILE* out, int index) {
    writePushSegment(out, "THIS", index);
}

static void codegen_push_that(FILE* out, int index) {
    writePushSegment(out, "THAT", index);
}

static void codegen_push_pointer(FILE* out, int index) {
    if (index == 0) {
        writePushPointer(out, "THIS");
    }
    else if (index == 1) {
        writePushPointer(out, "THAT");
    }
    else {
        printf("Incorrect use of \"pointer\" segment at line %d. The index must be 0 (this) or 1 (that).\n", yylineno);
        exit(1);
    }
}

static void codegen_push_temp(FILE* out, int index) {
    if (index < 0 || index > 7) {
        printf("Incorrect use of \"temp\" segment at line %d. The index varies from 0 to 7.\n", yylineno);
        exit(1);
    }

    emit("// begin instruction: push temp %d\n\n", index);
    
    emit("// D = mem[5 + i]\n");
    emit("@5\n");
    emit("D=A\n");
    emit("@%d\n", index);
    emit("A=D+A\n");
    emit("D=M\n\n");

    emit_push_d_to_stack();
    emit_increment_sp();

    emit("// end instruction: push temp %d\n\n", index);
}

static void codegen_push_constant(FILE* out, int index) {
    emit("// begin instruction: push constant %d\n\n", index);

    emit("// D = i\n");
    emit("@%d\n", index);
    emit("D=A\n\n");

    emit_push_d_to_stack();
    emit_increment_sp();

    emit("// end instruction: push constant %d\n\n", index);
}

static void codegen_push_static(FILE* out, int index) {
    char* static_var_name = get_static_variable_name(index);
    
    emit("// begin instruction: push static %d\n\n", index);

    emit("// D = mem[prog.i]\n");
    emit("@%s\n", static_var_name);
    emit("D=M\n\n");

    emit_push_d_to_stack();
    emit_increment_sp();

    emit("// end instruction: push static %d\n\n", index);

    free(static_var_name);
}

static void
writePushSegment(FILE* out, char* segment, int index) {
    emit("// begin instruction: push %s %d\n\n", segment, index);

    emit("// D = mem[seg + i]\n");
    emit("@%s\n", segment);
    emit("D=M\n");
    emit("@%d\n", index);
    emit("A=D+A\n");
    emit("D=M\n\n");

    emit_push_d_to_stack();
    emit_increment_sp();

    emit("// end instruction: push %s %d\n\n", segment, index);
}

static void
writePushPointer(FILE* out, char* segment) {
    emit("// begin instruction: push pointer %s\n\n", segment);
    
    emit("// D = mem[%s]\n", segment);
    emit("@%s\n", segment);
    emit("D=M\n\n");

    emit_push_d_to_stack();
    emit_increment_sp();
    
    emit("// end instruction: push pointer %s\n\n", segment);
}

static char*
make_combined_name(char* base, int index) {

    int size = snprintf(NULL, 0, "%s.%d", base, index) + 1;
    
    char* name = malloc(sizeof(char) * size);
    if (!name) {
        printf("Error allocating memory for combined name for static command.\n");
        exit(1);
    }

    snprintf(name, size, "%s.%d", base, index);
    return name;
}

static char*
get_prefix_before_dot(char* input) {
    const char* dot = strrchr(input, '.');

    if (!dot) {
        return strdup(input);
    }

    size_t len = dot - input;
    char* result = malloc(len + 1);
    if (!result) {
        perror("malloc");
        exit(1);
    }

    strncpy(result, input, len);
    result[len] = '\0';
    return result;
}

static char*
get_static_variable_name(int index) {
    char* filenamePrefix = get_prefix_before_dot(asm_filename);
    char* varName = make_combined_name(filenamePrefix, index);
    free(filenamePrefix);
    return varName;
}