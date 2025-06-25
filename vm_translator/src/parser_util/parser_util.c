#include "function_codegen.h"
#include "compute_codegen.h"
#include "memory_codegen.h"
#include "label_codegen.h"
#include "codegen_util.h"
#include "parser_util.h"
#include "file_util.h"
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static FILE*
get_asm_file();

static void
merge_and_delete_asm_file(char* filename, FILE* dest);

static void
checked_remove_file(char* filename);

void
vmparserUtil_open_out_file(char* filename) {

    if (!fileUtil_has_vm_extension(filename)) {
        printf("File %s is not a VM file.\n", filename);
        exit(1);
    }

    char* base_filename = fileUtil_get_basename(filename);
    char* asm_filename = fileUtil_append_asm(base_filename);

    size_t len = strlen(OUT_FOLDER) + 1 + strlen(asm_filename) + 1;  // "/" + null terminator
    char* out_path = malloc(len);
    snprintf(out_path, len, "%s/%s", OUT_FOLDER, asm_filename);
    mkdir(OUT_FOLDER, 0755);
    
    vm_context.input_filename = base_filename;
    OUT_FILE = fileUtil_open_file(out_path);

    free(asm_filename);
    free(out_path);
}

void
vmparserUtil_combine_asm_files() {

    int count;

    char** files    = fileUtil_get_files("./out", &count);
    FILE*  dest     = get_asm_file();

    for (int i = 0; i < count; i++) {
        if (fileUtil_has_asm_extension(files[i])) {
            merge_and_delete_asm_file(files[i], dest);
        }
        free(files[i]);
    }

    free(files);
    fclose(dest);
}

void
vmparserUtil_append_bootstrap_code() {
    char* output_filename = "out/exe.asm";

    FILE* temp_file = fileUtil_open_file("temp");
    generate_bootstrap_code(temp_file);

    FILE* existing_output_file = fopen(output_filename, "r");
    if (!existing_output_file) {
        perror("fopen");
        exit(1);
    }

    char buffer[1024];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), existing_output_file)) > 0) {
        fwrite(buffer, 1, bytes, temp_file);
    }

    fclose(temp_file);
    fclose(existing_output_file);


    if (rename("temp", output_filename) != 0) {
        perror("rename");
        exit(1);
    }
}

void
vmparserUtil_handleMemoryOperation(MemOp_T op, Segment_T seg, int index) {
    generate_memory_operation(OUT_FILE, op, seg, index);
}

void
vmparserUtil_handleComputeOperation(ComputeOp_T op) {
    generate_compute_operation(OUT_FILE, op);
}

void
vmparserUtil_handleLabelOperation(LabelOp_T op, char* label) {
    generate_label_operation(OUT_FILE, op, label);
}

void
vmparserUtil_handleFunctionOperation(char* func, int args) {
    generate_function_operation(OUT_FILE, func, args);
}

void
vmparserUtil_handleCallOperation(char* func, int locals) {
    generate_call_operation(OUT_FILE, func, locals);
}

void
vmparserUtil_handleReturnOperation() {
    generate_return_operation(OUT_FILE);
}

void
vmparserUtil_cleanup() {
    free(vm_context.input_filename);
    fclose(OUT_FILE);
}

static FILE*
get_asm_file() {
    char* asm_filename = "out/exe.asm";

    FILE* asm_file = fileUtil_open_file(asm_filename);

    return asm_file;
}

static void
merge_and_delete_asm_file(char* filename, FILE* dest) {
    fileUtil_append_file_contents(filename, dest);
    checked_remove_file(filename);
}

static void
checked_remove_file(char* filename) {
    if (remove(filename) != 0) {
        perror("remove");
        exit(1);
    }
}