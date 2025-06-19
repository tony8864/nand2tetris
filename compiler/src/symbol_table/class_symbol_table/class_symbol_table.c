#include "class_symbol_table.h"
#include "safe_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define CLASS_HASH_TABLE_SIZE 200

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

typedef struct ClassSymbolTable {
    ClassSymbolTableEntry* buckets[CLASS_HASH_TABLE_SIZE];
    unsigned staticCount;
    unsigned fieldCount;
} ClassSymbolTable;

static ClassVariable*
create_variable(char* name, ClassScopeType kind, VarType* type, unsigned index);

static ClassSymbolTableEntry*
create_entry(ClassVariable* variable);

static void
print_row(ClassVariable* variable);

static char*
class_scope_type_to_string(ClassScopeType type);

static void
free_entry(ClassSymbolTableEntry* entry);

static unsigned long
hash_string(char* str);

ClassSymbolTable*
classSymtab_initialize() {
    ClassSymbolTable* table = safe_malloc(sizeof(ClassSymbolTable));
    
    for (int i = 0; i < CLASS_HASH_TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }

    table->fieldCount  = 0;
    table->staticCount = 0;

    return table;
}

ClassSymbolTableEntry*
classSymtab_insert(ClassSymbolTable* table, char* name, ClassScopeType kind, VarType* type) {
    unsigned long hash = hash_string(name);

    unsigned index = (kind == STATIC_SCOPE) ? table->staticCount++ : table->fieldCount++;

    ClassVariable*          variable = create_variable(name, kind, type, index);
    ClassSymbolTableEntry*  entry    = create_entry(variable);
    ClassSymbolTableEntry*  head     = table->buckets[hash];

    entry->next = head;
    table->buckets[hash] = entry;

    return entry;
}

ClassSymbolTableEntry*
classSymtab_lookup(ClassSymbolTable* table, char* name) {
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

void
classSymtab_print(ClassSymbolTable* table) {
    printf("------------------- Class Symbol Table -------------------\n");
    printf("%-10s %-10s %-10s %-10s\n", "name", "kind", "type", "index");
    ClassSymbolTableEntry* curr;
    for (int i = 0; i < CLASS_HASH_TABLE_SIZE; i++) {
        if (table->buckets[i] != NULL) {
            curr = table->buckets[i];
            while (curr) {
                print_row(curr->variable);
                curr = curr->next;
            }
        }
    }
}

void
classSymtab_free(ClassSymbolTable* table) {
    ClassSymbolTableEntry* curr;
    ClassSymbolTableEntry* tmp;
    for (int i = 0; i < CLASS_HASH_TABLE_SIZE; i++) {
        if (table->buckets[i] != NULL) {
            curr = table->buckets[i];
            while (curr) {
                tmp = curr;
                curr = curr->next;
                free_entry(tmp);
            }
        }
    }
    free(table);
}

void
classSymtab_get_str_kind(ClassSymbolTableEntry* entry) {
    // switch (entry->variable=>kind) {
    //     case STATIC_SCOPE: return "static";
    //     case FIELD_SCOPE:  return "field";
    //     default:           return "unknown";
    // }
}

unsigned
classSymtab_get_entry_index(ClassSymbolTableEntry* entry) {
    return entry->variable->index;
}

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
create_entry(ClassVariable* variable) {
    ClassSymbolTableEntry* entry = safe_malloc(sizeof(ClassSymbolTableEntry));
    entry->variable = variable;
    entry->next = NULL;
    return entry;
}

static void
print_row(ClassVariable* variable) {
    assert(variable && variable->type);
    char* kind = class_scope_type_to_string(variable->kind);
    char* type = common_var_type_to_string(variable->type);
    printf("%-10s %-10s %-10s %-10u\n", variable->name, kind, type, variable->index);
}

static char*
class_scope_type_to_string(ClassScopeType type) {
    switch (type) {
        case STATIC_SCOPE: return "static";
        case FIELD_SCOPE:  return "field";
        default:           return "unknown";
    }
}

static void
free_entry(ClassSymbolTableEntry* entry) {
    assert(entry && entry->variable && entry->variable->type);
    common_free_vartype(entry->variable->type);
    free(entry->variable->name);
    free(entry->variable);
    free(entry);
}

static unsigned long
hash_string(char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % CLASS_HASH_TABLE_SIZE;
}