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

char*
classSymtab_get_str_kind(ClassSymbolTableEntry* entry);

unsigned
classSymtab_get_entry_index(ClassSymbolTableEntry* entry);

VarType*
classSymtab_get_vartype(ClassSymbolTableEntry* entry);

unsigned
classSymtab_get_fields_count(ClassSymbolTable* table);

#endif