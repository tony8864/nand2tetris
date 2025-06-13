#ifndef SAFE_UTIL_H
#define SAFE_UTIL_H

#include <dirent.h>
#include <stdio.h>

void*
safe_malloc(size_t size);

void*
safe_realloc(void* ptr, size_t size);

FILE*
safe_fopen(const char* path, const char* mode);

DIR*
safe_opendir(const char* path);

char*
safe_strdup(const char* s);

#endif