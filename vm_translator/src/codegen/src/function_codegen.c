#include "codegen_util.h"
#include "function_codegen.h"

#include <stdlib.h>
#include <string.h>

#include "common.h"

#define emit(...)   tracked_emit(out, __VA_ARGS__)
#define push_val(val)                            \
                    emit("// push %s\n", val);    \
                    emit("@%s\n", val);     \
                    emit("D=A\n");          \
                    emit("@SP\n");          \
                    emit("A=M\n");          \
                    emit("M=D\n");          \
                    emit("@SP\n");          \
                    emit("M=M+1\n\n");

#define save_segment(seg)                               \
                    emit("// save segment %s\n", seg);  \
                    emit("@%s\n", seg);     \
                    emit("D=M\n");          \
                    emit("@SP\n");          \
                    emit("A=M\n");          \
                    emit("M=D\n");          \
                    emit("@SP\n");          \
                    emit("M=M+1\n\n");


#define reposition_arg(args)    \
                    emit("// reposition arg\n"); \
                    emit("@SP\n");               \
                    emit("D=M\n");               \
                    emit("@5\n");                \
                    emit("D=D-A\n");             \
                    emit("@%d\n", args);         \
                    emit("D=D-A\n");             \
                    emit("@ARG\n");              \
                    emit("M=D\n\n");
 
#define reposition_lcl()    \
                    emit("// reposition lcl\n"); \
                    emit("@SP\n");               \
                    emit("D=M\n");               \
                    emit("@LCL\n");              \
                    emit("M=D\n\n");

#define goto_f(f)                           \
                    emit("// goto f\n");    \
                    emit("@%s\n", f);       \
                    emit("0;JMP\n\n");

#define inject_return_address(ret)          \
                    emit("// inject return address\n"); \
                    emit("(%s)\n\n", ret);

#define restore_segment(seg, offset) \
                    emit("// restore segment %s\n", seg);   \
                    emit("@R13\n");                         \
                    emit("D=M\n");                          \
                    emit("@%d\n", offset);                  \
                    emit("A=D-A\n");                        \
                    emit("D=M\n");                          \
                    emit("@%s\n", seg);                     \
                    emit("M=D\n\n");

static char*
create_return_address();

static char*
int_to_str(int n);

void
generate_bootstrap_code(FILE* out) {
    emit("// bootstrap code\n");
    emit("@256\n");
    emit("D=A\n");
    emit("@0\n");
    emit("M=D\n\n");

    generate_call_operation(out, "Sys.init", 0);
}

void
generate_function_operation(FILE* out, char* func, unsigned locals) {
    if (vm_context.current_function_name != NULL) {
        free(vm_context.current_function_name);
    }

    vm_context.current_function_name    = strdup(func);
    vm_context.return_address_count     = 0;

    emit("// begin function\n\n");

    emit("(%s)\n\n", func);

    for (int i = 0; i < locals; i++) {
        push_val("0");
    }

    emit("// end function\n\n");
}

void
generate_call_operation(FILE* out, char* func, unsigned args) {
    char* ret_address = create_return_address();
    vm_context.return_address_count++;

    emit("// begin call\n\n");
    
    push_val(ret_address);
    save_segment("LCL");
    save_segment("ARG");
    save_segment("THIS");
    save_segment("THAT");
    reposition_arg(args);
    reposition_lcl();
    goto_f(func);
    inject_return_address(ret_address);

    emit("// end call\n\n");

    free(ret_address);
}

void
generate_return_operation(FILE* out) {
    emit("// begin return\n\n");

    emit("// temp variable for LCL\n");
    emit("@LCL\n");
    emit("D=M\n");
    emit("@R13\n");
    emit("M=D\n\n");

    emit("// save ret addr in temp variable\n");
    emit("@R13\n");
    emit("D=M\n");
    emit("@5\n");
    emit("A=D-A\n");
    emit("D=M\n");
    emit("@R14\n");
    emit("M=D\n\n");

    emit("// reposition return value for the caller\n");
    emit("@SP\n");
    emit("AM=M-1\n");
    emit("D=M\n");
    emit("@ARG\n");
    emit("A=M\n");
    emit("M=D\n\n");

    emit("// reposition SP for the caller\n");
    emit("@ARG\n");
    emit("D=M+1\n");
    emit("@SP\n");
    emit("M=D\n\n");

    restore_segment("THAT", 1);
    restore_segment("THIS", 2);
    restore_segment("ARG", 3);
    restore_segment("LCL", 4);

    emit("// goto ret addr\n");
    emit("@R14\n");
    emit("A=M\n");
    emit("0;JMP\n\n");

    emit("// end return\n\n");
}

static char*
create_return_address() {
    char* ret   = "$ret.";
    char* func  = vm_context.current_function_name;
    char* count = int_to_str(vm_context.return_address_count);

    size_t size  = strlen(func) + strlen(ret) + strlen(count) + 1; // '\0' char
    char*  label = malloc(sizeof(char) * size);
    if (!label) {
        perror("malloc");
        exit(1);
    }

    sprintf(label, "%s%s%s", func, ret, count);
    free(count);
    return label;
}

static char*
int_to_str(int n) {
    char* buffer = malloc(12);
    if (!buffer) {
        perror("malloc");
        exit(1);
    }

    snprintf(buffer, 12, "%d", n);
    return buffer;
}