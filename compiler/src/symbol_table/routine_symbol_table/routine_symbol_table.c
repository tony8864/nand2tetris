#include "routine_symbol_table.h"
#include "safe_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ROUTINE_BUCKETS 200

typedef struct RoutineVariable {
    char*            name;
    RoutineScopeType kind;
    VarType*         type;
    unsigned         index;
} RoutineVariable;


typedef struct RoutineSymbolTableEntry {
    RoutineVariable* variable;
    RoutineSymbolTableEntry* next;
} RoutineSymbolTableEntry;

typedef struct RoutineSymbolTable {
    RoutineSymbolTableEntry* buckets[ROUTINE_BUCKETS];
    unsigned varCount;
    unsigned argCount;
} RoutineSymbolTable;

static RoutineVariable*
create_variable(char* name, RoutineScopeType kind, VarType* type, unsigned index);

static RoutineSymbolTableEntry*
create_entry(RoutineVariable* variable);

static void
print_row(RoutineVariable* variable);

static void
free_entry(RoutineSymbolTableEntry* entry);

static char*
routine_scope_type_to_string(RoutineScopeType type);

static unsigned long
hash_string(char* str);

RoutineSymbolTable*
routineSymtab_initialize() {
    RoutineSymbolTable* table = safe_malloc(sizeof(RoutineSymbolTable));

    for (int i = 0; i < ROUTINE_BUCKETS; i++) {
        table->buckets[i] = NULL;
    }

    table->varCount = 0;
    table->argCount = 0;

    return table;
}

RoutineSymbolTableEntry*
routineSymtab_insert(RoutineSymbolTable* table, char* name, RoutineScopeType kind, VarType* type) {
    unsigned long hash = hash_string(name);

    unsigned index = (kind == ARG_TYPE) ? table->argCount++ : table->varCount++;

    RoutineVariable*          variable = create_variable(name, kind, type, index);
    RoutineSymbolTableEntry*  entry    = create_entry(variable);
    RoutineSymbolTableEntry*  head     = table->buckets[hash];

    entry->next = head;
    table->buckets[hash] = entry;

    return entry;
}

RoutineSymbolTableEntry*
routineSymtab_lookup(RoutineSymbolTable* table, char* name) {
    unsigned long hash = hash_string(name);

    RoutineSymbolTableEntry* curr = table->buckets[hash];

    while (curr) {
        if (strcmp(curr->variable->name, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void
routineSymtab_print(RoutineSymbolTable* table) {
    printf("------------------- Routine Symbol Table -------------------\n");
    printf("%-10s %-10s %-10s %-10s\n", "name", "kind", "type", "index");
    RoutineSymbolTableEntry* curr;
    for (int i = 0; i < ROUTINE_BUCKETS; i++) {
        if (table->buckets[i] != NULL) {
            curr = table->buckets[i];
            while (curr) {
                print_row(curr->variable);
                curr = curr->next;
            }
        }
    }
    printf("\n");
}

void
routineSymtab_free(RoutineSymbolTable* table) {
    RoutineSymbolTableEntry* curr;
    RoutineSymbolTableEntry* tmp;
    for (int i = 0; i < ROUTINE_BUCKETS; i++) {
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

char*
routineSymtab_get_str_kind(RoutineSymbolTableEntry* entry) {
    switch (entry->variable->kind) {
        case ARG_TYPE: return "argument";
        case VAR_TYPE: return "local";
        default:           return "unknown";
    }
}

unsigned
routineSymtab_get_entry_index(RoutineSymbolTableEntry* entry) {
    return entry->variable->index;
}

static RoutineVariable*
create_variable(char* name, RoutineScopeType kind, VarType* type, unsigned index) {
    RoutineVariable* variable = safe_malloc(sizeof(RoutineVariable));
    VarType* copied_type    = common_copy_vartype(type);
    variable->name = safe_strdup(name);
    variable->kind = kind;
    variable->type = copied_type;
    variable->index = index;
    return variable;
}

static RoutineSymbolTableEntry*
create_entry(RoutineVariable* variable) {
    RoutineSymbolTableEntry* entry = safe_malloc(sizeof(RoutineSymbolTableEntry));
    entry->variable = variable;
    entry->next = NULL;
    return entry;
}

static void
print_row(RoutineVariable* variable) {
    assert(variable);
    char* kind = routine_scope_type_to_string(variable->kind);
    if (variable->type != NULL) {
        char* type = common_var_type_to_string(variable->type);
        printf("%-10s %-10s %-10s %-10u\n", variable->name, kind, type, variable->index);
    }
    else {
        printf("%-10s %-10s %-10s %-10u\n", variable->name, kind, "0", variable->index);
    }
}

static void
free_entry(RoutineSymbolTableEntry* entry) {
    if (entry == NULL) return;

    if (entry->variable != NULL) {
        if (entry->variable->type != NULL) {
            common_free_vartype(entry->variable->type);
        }
        free(entry->variable->name);
        free(entry->variable);
    }

    free(entry);
}

static char*
routine_scope_type_to_string(RoutineScopeType type) {
    switch (type) {
        case ARG_TYPE: return "arg";
        case VAR_TYPE: return "var";
        default:           return "unknown";
    }
}

static unsigned long
hash_string(char* str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % ROUTINE_BUCKETS;
}