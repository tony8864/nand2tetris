#ifndef ROUTINE_SYM_TAB_H
#define ROUTINE_SYM_TAB_H

#include "common.h"

RoutineSymbolTable*
routineSymtab_initialize();

RoutineSymbolTableEntry*
routineSymtab_insert(RoutineSymbolTable* table, char* name, RoutineScopeType kind, VarType* type);

RoutineSymbolTableEntry*
routineSymtab_lookup(RoutineSymbolTable* table, char* name);

void
routineSymtab_print(RoutineSymbolTable* table);

void
routineSymtab_free(RoutineSymbolTable* table);

char*
routineSymtab_get_str_kind(RoutineSymbolTableEntry* entry);

unsigned
routineSymtab_get_entry_index(RoutineSymbolTableEntry* entry);

VarType*
routineSymtab_get_vartype(RoutineSymbolTableEntry* entry);

unsigned
routineSymtab_get_locals_count(RoutineSymbolTable* table);

#endif