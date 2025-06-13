#ifndef FILE_UTIL_H
#define FILE_UTIL_H

unsigned
FILEUTIL_is_directory(const char* path);

char**
FILEUTIL_list_files(const char* path, int* count);

void
FILEUTIL_free_file_list(char** files, int count);

#endif