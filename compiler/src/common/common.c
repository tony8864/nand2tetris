#include "common.h"
#include "safe_util.h"

#include <stdlib.h>

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

void
common_free_vartype(VarType* t) {
    if (!t) return;

    if (t->kind == CLASS_TYPE && t->className) {
        free(t->className);
    }
    
    free(t);
}

char*
class_scope_type_to_string(ClassScopeType type) {
    switch (type) {
        case STATIC_SCOPE: return "static";
        case FIELD_SCOPE:  return "field";
        default:           return "unknown";
    }
}

char*
var_type_to_string(VarType* type) {
    VarTypeKind kind = type->kind;
    switch (kind) {
        case INT_TYPE:      return "int";
        case CHAR_TYPE:     return "char";
        case BOOLEAN_TYPE:  return "bool";
        case CLASS_TYPE:    return "class";
        default:            return "unknown";
    }
}