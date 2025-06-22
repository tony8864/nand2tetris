#ifndef CLASS_SYM_TAB_H
#define CLASS_SYM_TAB_H

#include "common.h"

// -----------------------------------------------------------------------------
// Initialization & Cleanup
// -----------------------------------------------------------------------------

ClassSymbolTable*
classSymtab_initialize();

void
classSymtab_free(ClassSymbolTable* table);

// -----------------------------------------------------------------------------
// Insertion & Lookup
// -----------------------------------------------------------------------------

ClassSymbolTableEntry*
classSymtab_insert_variable(ClassSymbolTable* table, char* name, ClassScopeType kind, VarType* type);

ClassSymbolTableEntry*
classSymtab_lookup_variable(ClassSymbolTable* table, char* name);

SubroutineEntry*
classSymtab_insert_routine(ClassSymbolTable* table, Subroutine* subroutine);

SubroutineEntry*
classSymtab_lookup_routine(ClassSymbolTable* table, char* name);

// -----------------------------------------------------------------------------
// Accessors
// -----------------------------------------------------------------------------

VarType*
classSymtab_get_vartype(ClassSymbolTableEntry* entry);

char*
classSymtab_get_str_kind(ClassSymbolTableEntry* entry);

unsigned
classSymtab_get_entry_index(ClassSymbolTableEntry* entry);

unsigned
classSymtab_get_fields_count(ClassSymbolTable* table);


void
classSymtab_print(ClassSymbolTable* table);

#endif // CLASS_SYM_TAB_H
