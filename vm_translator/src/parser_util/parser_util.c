#include "parser_util.h"

#include "memory_codegen.h"
#include "compute_codegen.h"
#include "label_codegen.h"

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
vmparserUtil_cleanup() {
    fileUtil_close_and_free_filename(vm_context.output_file, vm_context.input_filename);
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