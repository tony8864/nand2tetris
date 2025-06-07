#include "parser_util.h"

#include "memory_codegen.h"
#include "compute_codegen.h"
#include "file_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
vmparserUtil_handleMemoryOperation(MemOp_T op, Segment_T seg, int index) {
    generate_memory_operation(vm_context.output_file, op, seg, index);
}

void
vmparserUtil_handleComputeOperation(ComputeCommand_T cmd) {
    generate_compute_operation(vm_context.output_file, cmd);
}

void
vmparserUtil_cleanup() {
    fileUtil_close_and_free_filename(vm_context.output_file, vm_context.input_filename);
    printf("total instr: %d\n", vm_context.instruction_count);
}