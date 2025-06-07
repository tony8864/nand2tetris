#ifndef FILE_UTIL_H
#define FILE_UTIL_H

#include <stdio.h>

char*
fileUtil_get_basename(char* filename);

char*
fileUtil_append_asm(char* base_name);

FILE*
fileUtil_open_file(char* filename);

void
fileUtil_close_and_free_filename(FILE* file, char* filename);

unsigned
fileUtil_isDirectory(char* path);

void
fileUtil_list_files_in_directory(char* path);

char**
fileUtil_get_files(char* dir_path, int* count);

unsigned
fileUtil_has_vm_extension(char* filename);

#endif