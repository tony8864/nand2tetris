#include "class_symbol_table.h"
#include "safe_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define CLASS_SYMTAB_BUCKET_COUNT 200

typedef struct ClassVariable {
    char*           name;
    ClassScopeType  kind;
    VarType*        type;
    unsigned        index;
} ClassVariable;

typedef struct ClassSymbolTableEntry {
    ClassVariable* variable;
    ClassSymbolTableEntry* next;
} ClassSymbolTableEntry;

typedef struct SubroutineEntry {
    Subroutine* subroutine;
    SubroutineEntry* next;
} SubroutineEntry;

typedef struct ClassSymbolTable {
    ClassSymbolTableEntry* buckets[CLASS_SYMTAB_BUCKET_COUNT];
    SubroutineEntry* subroutineBuckets[CLASS_SYMTAB_BUCKET_COUNT];
    unsigned staticCount;
    unsigned fieldCount;
} ClassSymbolTable;

static ClassVariable*
create_variable(char* name, ClassScopeType kind, VarType* type, unsigned index);

static ClassSymbolTableEntry*
create_variable_entry(ClassVariable* variable);

static SubroutineEntry*
create_subroutine_entry(Subroutine* subroutine);

static void
print_variable_table(ClassSymbolTableEntry** buckets);

static void
print_routine_table(SubroutineEntry** buckets);

static void
print_variable(ClassVariable* variable);

static void
print_subroutine(Subroutine* subroutine);

static char*
class_scope_type_to_string(ClassScopeType type);

static char*
subroutine_type_to_string(SubroutineType type);

static void
free_variables(ClassSymbolTableEntry** buckets);

static void
free_variable_entry(ClassSymbolTableEntry* entry);

static void
free_subroutines(SubroutineEntry** buckets);

static void
free_subroutine_entry(SubroutineEntry* entry);

static unsigned long
hash_string(char* str);

// -----------------------------------------------------------------------------
// Initialization & Cleanup
// -----------------------------------------------------------------------------

ClassSymbolTable*
classSymtab_initialize() {
    ClassSymbolTable* table = safe_malloc(sizeof(ClassSymbolTable));
    
    for (int i = 0; i < CLASS_SYMTAB_BUCKET_COUNT; i++) {
        table->buckets[i] = NULL;
        table->subroutineBuckets[i] = NULL;
    }

    table->fieldCount  = 0;
    table->staticCount = 0;

    return table;
}

void
classSymtab_free(ClassSymbolTable* table) {
    free_variables(table->buckets);
    free_subroutines(table->subroutineBuckets);
    free(table);
}

// -----------------------------------------------------------------------------
// Insertion & Lookup
// -----------------------------------------------------------------------------

ClassSymbolTableEntry*
classSymtab_insert_variable(ClassSymbolTable* table, char* name, ClassScopeType kind, VarType* type) {
    unsigned long hash = hash_string(name);

    unsigned index = (kind == STATIC_SCOPE) ? table->staticCount++ : table->fieldCount++;

    ClassVariable*          variable = create_variable(name, kind, type, index);
    ClassSymbolTableEntry*  entry    = create_variable_entry(variable);
    ClassSymbolTableEntry*  head     = table->buckets[hash];

    entry->next = head;
    table->buckets[hash] = entry;

    return entry;
}

ClassSymbolTableEntry*
classSymtab_lookup_variable(ClassSymbolTable* table, char* name) {
    unsigned long hash = hash_string(name);

    ClassSymbolTableEntry* curr = table->buckets[hash];

    while (curr) {
        if (strcmp(curr->variable->name, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

SubroutineEntry*
classSymtab_insert_routine(ClassSymbolTable* table, Subroutine* subroutine) {
    unsigned long hash = hash_string(subroutine->name);

    SubroutineEntry* entry = create_subroutine_entry(subroutine);
    SubroutineEntry* head  = table->subroutineBuckets[hash];

    entry->next = head;
    table->subroutineBuckets[hash] = entry;

    return entry;
}

SubroutineEntry*
classSymtab_lookup_routine(ClassSymbolTable* table, char* name) {
    unsigned long hash = hash_string(name);

    SubroutineEntry* cur = table->subroutineBuckets[hash];

    while (cur) {
        if (strcmp(cur->subroutine->name, name) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

// -----------------------------------------------------------------------------
// Accessors
// -----------------------------------------------------------------------------

VarType*
classSymtab_get_vartype(ClassSymbolTableEntry* entry) {
    return entry->variable->type;
}

char*
classSymtab_get_str_kind(ClassSymbolTableEntry* entry) {
    switch (entry->variable->kind) {
        case STATIC_SCOPE: return "static";
        case FIELD_SCOPE:  return "this";
        default:           return "unknown";
    }
}

unsigned
classSymtab_get_entry_index(ClassSymbolTableEntry* entry) {
    return entry->variable->index;
}

unsigned
classSymtab_get_fields_count(ClassSymbolTable* table) {
    ClassSymbolTableEntry* curr;
    unsigned fields = 0;
    for (int i = 0; i < CLASS_SYMTAB_BUCKET_COUNT; i++) {
        if (table->buckets[i] != NULL) {
            curr = table->buckets[i];
            while (curr) {
                if (curr->variable->kind == FIELD_SCOPE) {
                    fields++;
                }
                curr = curr->next;
            }
        }
    }
    return fields;
}

SubroutineType
classSymtab_get_subroutine_type(SubroutineEntry* entry) {
    return entry->subroutine->type;
}

// -----------------------------------------------------------------------------
// Debugging
// -----------------------------------------------------------------------------

void
classSymtab_print(ClassSymbolTable* table) {
    print_variable_table(table->buckets);
    printf("\n\n");
    print_routine_table(table->subroutineBuckets);
}

// -----------------------------------------------------------------------------
// Static Definitions
// -----------------------------------------------------------------------------

static ClassVariable*
create_variable(char* name, ClassScopeType kind, VarType* type, unsigned index) {
    ClassVariable* variable = safe_malloc(sizeof(ClassVariable));
    VarType* copied_type    = common_copy_vartype(type);
    variable->name = safe_strdup(name);
    variable->kind = kind;
    variable->type = copied_type;
    variable->index = index;
    return variable;
}

static ClassSymbolTableEntry*
create_variable_entry(ClassVariable* variable) {
    ClassSymbolTableEntry* entry = safe_malloc(sizeof(ClassSymbolTableEntry));
    entry->variable = variable;
    entry->next = NULL;
    return entry;
}

static SubroutineEntry*
create_subroutine_entry(Subroutine* subroutine) {
    SubroutineEntry* entry = safe_malloc(sizeof(SubroutineEntry));
    entry->subroutine = subroutine;
    entry->next = NULL;
    return entry;
}

static void
print_variable_table(ClassSymbolTableEntry** buckets) {
    printf("------------------- Class Symbol Table (Variables) -------------------\n");
    printf("%-10s %-10s %-10s %-10s\n", "name", "kind", "type", "index");
    ClassSymbolTableEntry* curr;
    for (int i = 0; i < CLASS_SYMTAB_BUCKET_COUNT; i++) {
        if (buckets[i] != NULL) {
            curr = buckets[i];
            while (curr) {
                print_variable(curr->variable);
                curr = curr->next;
            }
        }
    }
}

static void
print_routine_table(SubroutineEntry** buckets) {
    printf("------------------- Class Symbol Table (Subroutines) -------------------\n");
    printf("%-20s %-25s %-20s\n", "name", "type", "return type");
    SubroutineEntry* cur;
    for (int i = 0; i < CLASS_SYMTAB_BUCKET_COUNT; i++) {
        if (buckets[i] != NULL) {
            cur = buckets[i];
            while (cur) {
                print_subroutine(cur->subroutine);
                cur = cur->next;
            }
        }
    }
}

static void
print_variable(ClassVariable* variable) {
    assert(variable && variable->type);
    char* kind = class_scope_type_to_string(variable->kind);
    char* type = common_var_type_to_string(variable->type);
    printf("%-10s %-10s %-10s %-10u\n", variable->name, kind, type, variable->index);
}

static void
print_subroutine(Subroutine* subroutine) {
    char* type = subroutine_type_to_string(subroutine->type);
    char* returnType = common_var_type_to_string(subroutine->returnType);
    printf("%-20s %-25s %-20s\n", subroutine->name, type, returnType);
}

static char*
class_scope_type_to_string(ClassScopeType type) {
    switch (type) {
        case STATIC_SCOPE: return "static";
        case FIELD_SCOPE:  return "field";
        default:           return "unknown";
    }
}

static char*
subroutine_type_to_string(SubroutineType type) {
    switch(type) {
        case CONSTRUCTOR_TYPE: return "constructor"; break;
        case FUNCTION_TYPE: return "function"; break;
        case METHOD_TYPE: return "method"; break;
        default: exit(1);
    }
}

static void
free_variables(ClassSymbolTableEntry** buckets) {
    ClassSymbolTableEntry* curr;
    ClassSymbolTableEntry* tmp;
    for (int i = 0; i < CLASS_SYMTAB_BUCKET_COUNT; i++) {
        if (buckets[i] != NULL) {
            curr = buckets[i];
            while (curr) {
                tmp = curr;
                curr = curr->next;
                free_variable_entry(tmp);
            }
        }
    }
}

static void
free_variable_entry(ClassSymbolTableEntry* entry) {
    assert(entry && entry->variable && entry->variable->type);
    common_free_vartype(entry->variable->type);
    free(entry->variable->name);
    free(entry->variable);
    free(entry);
}

static void
free_subroutines(SubroutineEntry** buckets) {
    SubroutineEntry* curr;
    SubroutineEntry* tmp;
    for (int i = 0; i < CLASS_SYMTAB_BUCKET_COUNT; i++) {
        if (buckets[i] != NULL) {
            curr = buckets[i];
            while (curr) {
                tmp = curr;
                curr = curr->next;
                free_subroutine_entry(tmp);
            }
        }
    }
}

static void
free_subroutine_entry(SubroutineEntry* entry) {
    assert(entry && entry->subroutine);
    common_free_vartype(entry->subroutine->returnType);
    free(entry->subroutine->name);
    free(entry->subroutine);
    free(entry);
}

static unsigned long
hash_string(char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % CLASS_SYMTAB_BUCKET_COUNT;
}