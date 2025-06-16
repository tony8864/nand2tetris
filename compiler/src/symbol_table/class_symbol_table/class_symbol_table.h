#ifndef CLASS_SYM_TAB_H
#define CLASS_SYM_TAB_H

#include "common.h"

ClassSymbolTable*
classSymtab_initialize();

ClassSymbolTableEntry*
classSymtab_insert(ClassSymbolTable* table, char* name, ClassScopeType kind, VarType* type);

ClassSymbolTableEntry*
classSymtab_lookup(ClassSymbolTable* table, char* name);

void
classSymtab_print(ClassSymbolTable* table);

void
classSymtab_free(ClassSymbolTable* table);

#endif