#include "parser_util.h"
#include "memory_codegen.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

FILE* outFile       = NULL;
char* asm_filename  = NULL;

static char*
vm_to_asm_filename(char* vmfilename);

static void
openOutfile(char* filename);

void
vmparserUtil_openOutfile(char* vmfilename) {
    asm_filename = vm_to_asm_filename(vmfilename);
    openOutfile(asm_filename);
}

void
vmparserUtil_handleMemoryOperation(MemOp_T op, Segment_T seg, int index) {
    generate_memory_operation(outFile, op, seg, index);
}

void
vmparserUtil_cleanup() {
    if (outFile) {
        fclose(outFile);
        outFile = NULL;
    }
    free(asm_filename);
    asm_filename = NULL;
}

static
char* vm_to_asm_filename(char* vmfilename) {
    // Get the base filename (after last slash)
    const char* slash = strrchr(vmfilename, '/');
    const char* base = slash ? slash + 1 : vmfilename;

    size_t len = strlen(base);

    // Validate: must end with ".vm" and have something before it
    if (len <= 3 || strcmp(base + len - 3, ".vm") != 0) {
        fprintf(stderr, "Error: Input file must end with .vm and contain at least one character before the extension.\n");
        exit(EXIT_FAILURE);
    }

    // Calculate new length: base - ".vm" + ".asm" + null terminator
    size_t asm_len = (len - 3) + 4 + 1;
    char* asm_filename = malloc(asm_len);
    if (!asm_filename) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // Copy prefix and append ".asm"
    snprintf(asm_filename, asm_len, "%.*s.asm", (int)(len - 3), base);

    return asm_filename;
}

static void
openOutfile(char* filename) {
    if (!(outFile = fopen(filename, "w"))) {
        printf("Error creating asm output file.\n");
        exit(1);
    }
}