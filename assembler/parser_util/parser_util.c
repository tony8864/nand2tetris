#include "../symbol_table/symbol_table.h"
#include "parser_util.h"

#include <stdio.h>
#include <stdlib.h>

SymbolTable* table = NULL;

static void
installPredefinedSymbols();

static void
installRegisterSymbols();

void
parserUtil_initialize() {
    table = symtab_initialize();

    installPredefinedSymbols();
}

void
parserUtil_cleanup() {
    symtab_print(table);
}

static void
installPredefinedSymbols() {
    symtab_insert(table, "SP", 0);
    symtab_insert(table, "LCL", 1);
    symtab_insert(table, "ARG", 2);
    symtab_insert(table, "THIS", 3);
    symtab_insert(table, "THAT", 4);
    symtab_insert(table, "SCREEN", 16384);
    symtab_insert(table, "KBD", 24576);

    installRegisterSymbols();
}

static void
installRegisterSymbols() {
    symtab_insert(table, "R0", 0);
    symtab_insert(table, "R1", 1);
    symtab_insert(table, "R2", 2);
    symtab_insert(table, "R3", 3);
    symtab_insert(table, "R4", 4);
    symtab_insert(table, "R5", 5);
    symtab_insert(table, "R6", 6);
    symtab_insert(table, "R7", 7);
    symtab_insert(table, "R8", 8);
    symtab_insert(table, "R9", 9);
    symtab_insert(table, "R10", 10);
    symtab_insert(table, "R11", 11);
    symtab_insert(table, "R12", 12);
    symtab_insert(table, "R13", 13);
    symtab_insert(table, "R14", 14);
    symtab_insert(table, "R15", 15);
}