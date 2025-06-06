#include "file_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

char*
fileUtil_get_basename(char* filepath) {
    const char* slash = strrchr(filepath, '/');
    const char* base = slash ? slash + 1 : filepath;

    const char* dot = strrchr(base, '.');
    size_t name_len = dot ? (size_t)(dot - base) : strlen(base);

    char* name = malloc(name_len + 1);
    if (!name) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strncpy(name, base, name_len);
    name[name_len] = '\0';

    return name;
}

char*
fileUtil_append_asm(char* base_name) {
    const char* extension = ".asm";
    size_t base_len = strlen(base_name);
    size_t ext_len = strlen(extension);

    char* result = malloc(base_len + ext_len + 1);
    if (!result) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    strcpy(result, base_name);
    strcat(result, extension);

    return result;
}

FILE*
fileUtil_open_file(char* filename) {
    FILE* out;
    assert(filename);
    if (!(out = fopen(filename, "w"))) {
        printf("Error creating \"%s\" output file.\n", filename);
        exit(1);
    }
    return out;
}

void
fileUtil_close_and_free_filename(FILE* file, char* filename) {
    assert(filename && file);
    fclose(file);
    free(filename);
}