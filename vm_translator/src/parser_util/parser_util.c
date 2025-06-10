#include "parser_util.h"

#include "memory_codegen.h"
#include "compute_codegen.h"
#include "label_codegen.h"
#include "function_codegen.h"
#include "codegen_util.h"

#include "file_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static FILE*
get_asm_file(char* filename);

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
    
    vm_context.input_filename = base_filename;
    vm_context.output_file = fileUtil_open_file(asm_filename);

    free(asm_filename);
}

void
vmparserUtil_combine_asm_files(char* filename) {

    int count;

    char** files    = fileUtil_get_files("./", &count);
    FILE*  dest     = get_asm_file(filename);

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
vmparserUtil_append_bootstrap_code(char* filename) {
    char* stripped_slash = fileUtil_strip_last_slash(filename);
    char* output_filename = fileUtil_append_asm(stripped_slash);

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

    free(stripped_slash);
    free(output_filename);
}

void
vmparserUtil_write_to_folder(char* filename) {
    char* stripped_slash = fileUtil_strip_last_slash(filename);
    char* output_filename = fileUtil_append_asm(stripped_slash);

    FILE* existing_output_file = fopen(output_filename, "r");
    if (!existing_output_file) {
        perror("fopen");
        exit(1);
    }

    char* output_folder_name = fileUtil_strip_last_slash(vm_context.output_folder_name);
    unsigned isDirectory = fileUtil_isDirectory(output_folder_name);

    if (!isDirectory) {
        printf("Error: Please specify a valid folder path.\n");
        exit(1);
    }

    size_t dest_len = strlen(output_folder_name) + strlen(output_filename) + 2;
    char* dest_path = malloc(dest_len);
    if (!dest_path) {
        perror("malloc");
        exit(1);
    }
    snprintf(dest_path, dest_len, "%s/%s", output_folder_name, output_filename);

    FILE* dest_file = fopen(dest_path, "w");
    if (!dest_file) {
        perror("fopen (dest)");
        free(dest_path);
        exit(1);
    }

    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), existing_output_file)) > 0) {
        fwrite(buffer, 1, bytes, dest_file);
    }

    if (remove(output_filename) != 0) {
        perror("remove");
        exit(1);
    }

    fclose(existing_output_file);
    fclose(dest_file);
    free(dest_path);
    free(stripped_slash);
    free(output_filename);
    free(output_folder_name);
}

void
vmparserUtil_handleMemoryOperation(MemOp_T op, Segment_T seg, int index) {
    generate_memory_operation(vm_context.output_file, op, seg, index);
}

void
vmparserUtil_handleComputeOperation(ComputeOp_T op) {
    generate_compute_operation(vm_context.output_file, op);
}

void
vmparserUtil_handleLabelOperation(LabelOp_T op, char* label) {
    generate_label_operation(vm_context.output_file, op, label);
}

void
vmparserUtil_handleFunctionOperation(char* func, int args) {
    generate_function_operation(vm_context.output_file, func, args);
}

void
vmparserUtil_handleCallOperation(char* func, int locals) {
    generate_call_operation(vm_context.output_file, func, locals);
}

void
vmparserUtil_handleReturnOperation() {
    generate_return_operation(vm_context.output_file);
}

void
vmparserUtil_cleanup() {
    free(vm_context.input_filename);
    fclose(vm_context.output_file);
}

static FILE*
get_asm_file(char* filename) {
    char* stripped      = fileUtil_strip_last_slash(filename);
    char* asm_filename  = fileUtil_append_asm(stripped);
    FILE* asm_file      = fileUtil_open_file(asm_filename);

    free(stripped);
    free(asm_filename);

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