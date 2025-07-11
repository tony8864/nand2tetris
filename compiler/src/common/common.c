#include "common.h"
#include "safe_util.h"

#include <stdlib.h>
#include <string.h>

CompilerContext gbl_context = {0};

unsigned lex_debug_mode = 0;
unsigned bison_debug_mode = 0;

void
common_set_lex_debug_mode(unsigned enabled) {
    lex_debug_mode = enabled;
}

void
common_set_bison_debug_mode(unsigned enabled) {
    bison_debug_mode = enabled;
}

VarType*
common_create_vartype(VarTypeKind kind, char* name) {
    VarType* type = safe_malloc(sizeof(VarType));
    type->kind = kind;
    type->className = name;
    return type;
}

VarType*
common_copy_vartype(VarType* original) {
    if (!original) return NULL;

    VarType* copy = safe_malloc(sizeof(VarType));
    copy->kind = original->kind;

    if (original->kind == CLASS_TYPE && original->className) {
        copy->className = safe_strdup(original->className);
    }
    else {
        copy->className = NULL;
    }

    return copy;
}

char*
common_get_classname_from_type(VarType* type) {
    return type->className;
}

void
common_free_vartype(VarType* t) {
    if (!t) return;

    if (t->kind == CLASS_TYPE && t->className) {
        free(t->className);
    }
    
    free(t);
}

char*
common_var_type_to_string(VarType* type) {
    if (!type) return "void";
    VarTypeKind kind = type->kind;
    switch (kind) {
        case INT_TYPE:      return "int";
        case CHAR_TYPE:     return "char";
        case BOOLEAN_TYPE:  return "bool";
        case CLASS_TYPE:    return "class";
        default:            return "unknown";
    }
}

unsigned
common_is_class_name(char* name) {
    for (int i = 0; i < JACK_CLASSES_COUNT; i++) {

        if (strcmp(name, JACK_CLASSES[i]) == 0) {
            return 1;
        }
    }
    return 0;
}