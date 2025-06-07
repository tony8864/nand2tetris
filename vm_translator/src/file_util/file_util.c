#include "file_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>

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

unsigned
fileUtil_isDirectory(char* path) {
    struct stat stats;

    stat(path, &stats);

    return S_ISDIR(stats.st_mode);
}

void
fileUtil_list_files_in_directory(char* path) {
    DIR* dir = opendir(path);

    if (!dir) {
        perror("opendir");
        exit(1);
    }

    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s%s", path, entry->d_name);

        struct stat st;
        if (stat(fullpath, &st) == 0 && S_ISREG(st.st_mode)) {
            printf("Found file: %s\n", fullpath);
        }
    }

    closedir(dir);
}

char**
fileUtil_get_files(char* path, int* count) {
    DIR* dir = opendir(path);

    if (!dir) {
        perror("opendir");
        exit(1);
    }

    struct dirent* entry;

    char** file_list = malloc(sizeof(char*) * 10);
    int size = 0;

    if (!file_list) {
        perror("malloc");
        exit(1);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s%s", path, entry->d_name);

        struct stat st;
        if (stat(fullpath, &st) == 0 && S_ISREG(st.st_mode)) {
            file_list[size++] = strdup(fullpath);
        }
    }
    closedir(dir);
    *count = size;
    return file_list;
}

unsigned
fileUtil_has_vm_extension(char* filename) {
    assert(filename);

    int len = strlen(filename);
    return (len >= 3 && strcmp(filename + len - 3, ".vm") == 0);
}