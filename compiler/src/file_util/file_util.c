#include "file_util.h"
#include "safe_util.h"
#include "str_util.h"
#include "common.h"

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

static int
is_special_dir(const char* name);

static char*
normalize_folder(const char* folder);

static char*
build_full_path(const char* folder, const char* file);

unsigned
FILEUTIL_is_directory(const char* path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return 0;
    }
    return S_ISDIR(path_stat.st_mode);
}

char**
FILEUTIL_list_files(const char* path, int* count) {
    char* normalized_folder = normalize_folder(path);
    DIR* dir = safe_opendir(normalized_folder);
    
    struct dirent* entry;
    char** file_list = NULL;
    int capacity = 10;
    int n = 0;

    file_list = safe_malloc(sizeof(char*) * capacity);


    
    while ((entry = readdir(dir)) != NULL) {
        if (is_special_dir(entry->d_name)) {
            continue;
        }

        char* full_path = build_full_path(normalized_folder, entry->d_name);

        if (n >= capacity) {
            capacity *= 2;
            file_list = safe_realloc(file_list, sizeof(char*) * capacity);
        }

        file_list[n++] = full_path;
    }

    closedir(dir);
    free(normalized_folder);
    *count = n;
    return file_list;
}

void
FILEUTIL_collect_jack_class_names(char** files, int count) {
    char* src_name = NULL;

    JACK_CLASSES = safe_malloc(sizeof(char*) * count);
    JACK_CLASSES_COUNT = count;

    for (int i = 0; i < count; i++) {
        src_name = strutil_path_to_source_name(files[i]);
        JACK_CLASSES[i] = src_name;
    }
}

void
FILEUTIL_free_jack_class_names(int count) {
    for (int i = 0; i < count; i++) {
        free(JACK_CLASSES[i]);
    }
    free(JACK_CLASSES);
}

void
FILEUTIL_print_jack_class_names() {
    char* class;
    for (int i = 0; i < JACK_CLASSES_COUNT; i++) {
        class = JACK_CLASSES[i];
        printf("class: %s\n", class);
    }
}

void
FILEUTIL_create_vm_file() {
    size_t len = strlen(OUT_FOLDER) + strlen(SRC_NAME) + 3 + 1 + 1; // + 3 for ".vm" + 1 for '/' +1 for '\0'
    char* vm_name = safe_malloc(len + 1);

    snprintf(vm_name, len, "%s/%s.vm", OUT_FOLDER, SRC_NAME);
    vm_name[len - 1] ='\0';

    mkdir(OUT_FOLDER, 0755);
    VM_NAME = vm_name;
    VM_FILE = safe_fopen(vm_name, "w");
}

void
FILEUTIL_free_file_list(char** files, int count) {
    for (int i = 0; i < count; i++) {
        free(files[i]);
    }
    free(files);
}

static int
is_special_dir(const char* name) {
    return (strcmp(name, ".") == 0 || strcmp(name, "..") == 0);
}

static char*
normalize_folder(const char* folder) {
    size_t len = strlen(folder);

    if (len > 0 && folder[len - 1] == '/') {
        char* normalized = safe_malloc(len);
        strncpy(normalized, folder, len - 1);
        normalized[len - 1] = '\0';
        return normalized;
    }

    return safe_strdup(folder);
}

static char*
build_full_path(const char* folder, const char* file) {
    size_t path_len = strlen(folder) + strlen(file) + 2;
    char* full_path = safe_malloc(path_len);
    snprintf(full_path, path_len, "%s/%s", folder, file);
    return full_path;
}