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

static char*
create_unique_func_name(char* func);

static char*
create_return_address();

static char*
int_to_str(int n);

void
generate_function_operation(FILE* out, char* func, unsigned locals) {
    char* unique_func_name = create_unique_func_name(func);

    if (vm_context.current_function_name != NULL) {
        free(vm_context.current_function_name);
    }

    vm_context.current_function_name    = strdup(func);
    vm_context.return_address_count     = 0;

    emit("// begin function\n\n");

    emit("(%s)\n\n", unique_func_name);

    for (int i = 0; i < locals; i++) {
        push_val("0");
    }

    emit("// end function\n\n");

    free(unique_func_name);
}

void
generate_call_operation(FILE* out, char* func, unsigned args) {
    char* ret_address = create_return_address();
    char* callee      = create_unique_func_name(func);

    vm_context.return_address_count++;

    emit("// begin call\n\n");
    
    push_val(ret_address);
    save_segment("LCL");
    save_segment("ARG");
    save_segment("THIS");
    save_segment("THAT");
    reposition_arg(args);
    reposition_lcl();
    goto_f(callee);
    inject_return_address(ret_address);

    emit("// end call\n\n");

    free(ret_address);
    free(callee);
}

void
generate_return_operation(FILE* out) {
    printf("return\n");
}

static char*
create_unique_func_name(char* func) {
    char* file = vm_context.input_filename;

    size_t size = strlen(file) + strlen(func) + 1 + 1; //'.' and '\0' chars
    char* unique = malloc(sizeof(char) * size);
    if (!unique) {
        perror("malloc");
        exit(1);
    }

    sprintf(unique, "%s.%s", file, func);
    return unique;
}

static char*
create_return_address() {
    char* ret   = "$ret.";
    char* file  = vm_context.input_filename;
    char* func  = vm_context.current_function_name;
    char* count = int_to_str(vm_context.return_address_count);

    size_t size  = strlen(file) + strlen(func) + strlen(ret) + strlen(count) + 1 + 1; //'.' and '\0' chars
    char*  label = malloc(sizeof(char) * size);
    if (!label) {
        perror("malloc");
        exit(1);
    }

    sprintf(label, "%s.%s%s%s", file, func, ret, count);
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