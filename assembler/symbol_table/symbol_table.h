#ifndef SYM_TAB
#define SYM_TAB

typedef struct SymbolTable SymbolTable;

SymbolTable*
symtab_initialize();

void
symtab_insert(SymbolTable* table, char* key, int value);

void
symtab_print(SymbolTable* table);


#endif