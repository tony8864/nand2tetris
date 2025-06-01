#ifndef SYM_TAB
#define SYM_TAB

typedef struct SymbolTable SymbolTable;

SymbolTable*
symtab_initialize();

void
symtab_insert(SymbolTable* table, char* key, int value);

unsigned
symtab_exists(SymbolTable* table, char* key);

int
symtab_getValue(SymbolTable* table, char* key);

void
symtab_print(SymbolTable* table);

void
symtab_cleanup(SymbolTable* table);


#endif