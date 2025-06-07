#include "codegen_util.h"
#include "compute_codegen.h"

#include <stdio.h>
#include <stdlib.h>

#define emit(...)               tracked_emit(out, __VA_ARGS__)
#define emit_increment_sp()                                    \
                                emit("// SP++\n");             \
                                emit("@SP\n");                 \
                                emit("M=M+1\n\n");

#define emit_decrement_sp()                                    \
                                emit("// SP--\n");             \
                                emit("@SP\n");                 \
                                emit("M=M-1\n\n");

#define emit_pop_y_to_D()                                      \
                                emit("// D = y\n");            \
                                emit("A=M\n");                 \
                                emit("D=M\n\n");
                                
#define emit_pop_x_to_A()                                       \
                                emit("// A = x\n");             \
                                emit("A=A-1\n");                \
                                emit("A=M\n\n");         

#define emit_push_D()                                           \
                                emit("// mem[top] = D\n");      \
                                emit("@SP\n");                  \
                                emit("A=M\n");                  \
                                emit("A=A-1\n");                \
                                emit("M=D\n\n");

#define emit_add()                                              \
                                emit("// D = y+x\n");           \
                                emit("D=D+A\n\n");
                                
#define emit_sub()                                           \
                                emit("// D = x - y\n");      \
                                emit("D=-D\n");              \
                                emit("D=D+A\n\n");

#define emit_and()                                          \
                                emit("// D = x & y\n");     \
                                emit("D=D&A\n\n");

#define emit_or()                                           \
                                emit("// D = x | y\n");     \
                                emit("D=D|A\n\n");

#define emit_before_jump()                                  \
                                emit("// D = y\n");         \
                                emit("@SP\n");              \
                                emit("AM=M-1\n");           \
                                emit("D=M\n\n");            \
                                emit("// D = x - y\n");     \
                                emit("@SP\n");              \
                                emit("AM=M-1\n");           \
                                emit("D=M-D\n\n");          \
                                emit("// if (cond) goto TRUE\n"); \
                                emit("@TRUE$%d\n", id);     \

#define emit_after_jump()                                   \
                                emit("// push 0\n");        \
                                emit("@SP\n");              \
                                emit("A=M\n");              \
                                emit("M=0\n\n");            \
                                emit("// goto END\n");      \
                                emit("@END$%d\n", id);      \
                                emit("0;JMP\n\n");          \
                                emit("// push -1\n");       \
                                emit("(TRUE$%d)\n", id);    \
                                emit("@SP\n");              \
                                emit("A=M\n");              \
                                emit("M=-1\n\n");           \
                                emit("// inc sp\n");        \
                                emit("(END$%d)\n", id);     \
                                emit("@SP\n");              \
                                emit("M=M+1\n\n");          \

static void codegen_add (FILE* out);
static void codegen_sub (FILE* out);
static void codegen_neg (FILE* out);
static void codegen_eq  (FILE* out);
static void codegen_gt  (FILE* out);
static void codegen_lt  (FILE* out);
static void codegen_and (FILE* out);
static void codegen_or  (FILE* out);
static void codegen_not (FILE* out);

static unsigned
get_unique_label_id();

typedef void (*compute_codegen_func_t)(FILE* out);

compute_codegen_func_t computeCodegenFuncs[] = {
    codegen_add,
    codegen_sub,
    codegen_neg,
    codegen_eq,
    codegen_gt,
    codegen_lt,
    codegen_and,
    codegen_or,
    codegen_not
};

void
generate_compute_operation(FILE* out, ComputeOp_T cmd) {
    (*computeCodegenFuncs[cmd])(out);
}

static void codegen_add(FILE* out) {

    emit("// begin add\n\n");

    emit_decrement_sp();
    emit_pop_y_to_D();
    emit_pop_x_to_A();
    emit_add();
    emit_push_D();

    emit("// end add\n\n");
}

static void codegen_sub(FILE* out) {
    emit("// begin sub\n\n");

    emit_decrement_sp();
    emit_pop_y_to_D();
    emit_pop_x_to_A();
    emit_sub();
    emit_push_D();

    emit("// end sub\n\n");
}

static void codegen_neg(FILE* out) {
    emit("// begin neg\n\n");

    emit("// D = y\n");
    emit("@SP\n");
    emit("A=M-1\n");
    emit("D=M\n\n");

    emit("// D = -y\n");
    emit("D=-D\n\n");

    emit("// M[top] = D\n");
    emit("M=D\n\n");

    emit("// end neg\n\n");
}

static void codegen_eq(FILE* out) {
    unsigned id = get_unique_label_id();

    emit("// begin eq\n\n");

    emit_before_jump()
    emit("D;JEQ\n\n");
    emit_after_jump();

    emit("// end eq\n\n");
}

static void codegen_gt(FILE* out) {
    unsigned id = get_unique_label_id();

    emit("// begin gt\n\n");

    emit_before_jump()
    emit("D;JGT\n\n");
    emit_after_jump();

    emit("// end gt\n\n");
}

static void codegen_lt(FILE* out) {
    unsigned id = get_unique_label_id();

    emit("// begin lt\n\n");

    emit_before_jump()
    emit("D;JLT\n\n");
    emit_after_jump();

    emit("// end lt\n\n");
}

static void codegen_and(FILE* out) {
    emit("// begin and\n\n");

    emit_decrement_sp();
    emit_pop_y_to_D();
    emit_pop_x_to_A();
    emit_and();
    emit_push_D();

    emit("// end and\n\n");
}

static void codegen_or(FILE* out) {
    emit("// begin or\n\n");

    emit_decrement_sp();
    emit_pop_y_to_D();
    emit_pop_x_to_A();
    emit_or();
    emit_push_D();

    emit("// end or\n\n");
}

static void codegen_not(FILE* out) {
    emit("// begin not\n\n");

    emit("// D = y\n");
    emit("@SP\n");
    emit("A=M-1\n");
    emit("D=M\n\n");

    emit("// D = not y\n");
    emit("D=!D\n\n");

    emit("// M[top] = D\n");
    emit("M=D\n\n");

    emit("// end not\n\n");
}

static unsigned
get_unique_label_id() {
    static unsigned counter = 0;
    return counter++;
}
