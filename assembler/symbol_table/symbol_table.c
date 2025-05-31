#include "symbol_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_SIZE 1024

typedef struct SymbolTableEntry {
    char* key;
    int value;
    struct SymbolTableEntry* next;
} SymbolTableEntry;

typedef struct SymbolTable {
    SymbolTableEntry* hashTable[HASH_TABLE_SIZE];
} SymbolTable;

static unsigned long
hash_djb2(const char *str);

static SymbolTableEntry*
initialize_entry(char* string, int value);

SymbolTable*
symtab_initialize() {
    SymbolTable* table;

    table = malloc(sizeof(SymbolTable));

    if (!table) {
        printf("Error allocating memory for symbol table.\n");
    }

    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        table->hashTable[i] = NULL;
    }

    return table;
}

void
symtab_insert(SymbolTable* table, char* key, int value) {
    SymbolTableEntry* entry;
    unsigned long hash;

    hash = hash_djb2(key);
    entry = initialize_entry(key, value);

    entry->next = table->hashTable[hash];
    table->hashTable[hash] = entry;
}

void
symtab_print(SymbolTable* table) {
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        if (table->hashTable[i] != NULL) {
            SymbolTableEntry* curr = table->hashTable[i];
            printf("hash: %d\n", i);
            while (curr) {
                printf("(k: %s, v:%d) ", curr->key, curr->value);
                curr = curr->next;
            }
            printf("\n");
        }
    }
}

static unsigned long
hash_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % HASH_TABLE_SIZE;
}

static SymbolTableEntry*
initialize_entry(char* key, int value) {
    SymbolTableEntry* entry;
    entry = malloc(sizeof(SymbolTableEntry));
    if (!entry) {
        printf("Error allocating memory for symbol table entry.\n");
        exit(1);
    }
    entry->key = strdup(key);
    entry->value = value;
}