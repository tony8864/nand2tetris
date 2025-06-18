#include "str_util.h"
#include "safe_util.h"

#include <string.h>
#include <assert.h>

char*
strutil_path_to_source_name(char* path) {

    char* slash = strrchr(path, '/');
    assert(slash);
    char* base = slash + 1;
    char* dot  = strrchr(base, '.');
    assert(dot);
    size_t len = (size_t)(dot - base);

    char* name = safe_malloc(len + 1);
    strncpy(name, base, len);
    name[len] = '\0';

    return name;
}

char*
strutil_strip_trailing_slash(char* path) {
    if (!path) return NULL;

    char* copy = safe_strdup(path);
    size_t len = strlen(copy);
    if (len > 1 && copy[len - 1] == '/') {
        copy[len - 1] = '\0';
    }
    return copy;
}