#include "safe_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void*
safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        perror("malloc");
        exit(1);
    }
    return ptr;
}

void*
safe_realloc(void* ptr, size_t size) {
    void* newPtr = realloc(ptr, size);
    if (!newPtr) {
        perror("realloc");
        exit(1);
    }
    return newPtr;
}

FILE*
safe_fopen(const char* path, const char* mode) {
    FILE* file = fopen(path, mode);
    if (!file) {
        perror("fopen");
        exit(1);
    }
    return file;
}

DIR*
safe_opendir(const char* path) {
    DIR* dir = opendir(path);
    if (!dir) {
        perror("opendir");
        exit(1);
    }
    return dir;
}

char*
safe_strdup(const char* s) {
    char* copy = strdup(s);
    if (!copy) {
        perror("copy");
        exit(1);
    }
    return copy;
}