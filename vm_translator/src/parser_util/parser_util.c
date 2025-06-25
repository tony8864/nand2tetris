#include "function_codegen.h"
#include "parser_util.h"
#include "file_util.h"

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static void validate_vm_file(char* filename);
static char* build_output_path(char* filename);

static FILE*
get_final_asm_file();

static void
merge_and_delete_asm_file(char* filename, FILE* dest);

static void
checked_remove_file(char* filename);

void
vmparserUtil_open_out_file(char* filename) {

    validate_vm_file(filename);

    char* base_filename = fileUtil_get_basename(filename);
    char* output_path = build_output_path(base_filename);
    
    vm_context.input_filename = base_filename;
    OUT_FILE = fileUtil_open_file(output_path);

    free(output_path);
}

void
vmparserUtil_combine_asm_files() {
    int count;
    char** files    = fileUtil_get_files("./out", &count);
    FILE*  dest     = get_final_asm_file();

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
vmparserUtil_cleanup() {
    free(vm_context.input_filename);
    fclose(OUT_FILE);
}

static void
validate_vm_file(char* filename) {
    if (!fileUtil_has_vm_extension(filename)) {
        printf("File %s is not a VM file.\n", filename);
        exit(1);
    }
}

static char*
build_output_path(char* filename) {
    char* asm_filename = fileUtil_append_asm(filename);
    size_t len = strlen(OUT_FOLDER) + 1 + strlen(asm_filename) + 1;
    char* out_path = malloc(len);
    snprintf(out_path, len, "%s/%s", OUT_FOLDER, asm_filename);
    mkdir(OUT_FOLDER, 0755);
    free(asm_filename);
    return out_path;
}

static FILE*
get_final_asm_file() {
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