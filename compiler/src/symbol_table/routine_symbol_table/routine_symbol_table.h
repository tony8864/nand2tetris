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

#endif