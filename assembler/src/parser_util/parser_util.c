#include "symbol_table.h"
#include "parser_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define A_INSTRUCTION_MAX   32767
#define WORD_SIZE           16
#define HACK_VARIABLE_BASE  16


#define INCREMENT_INSTRUCTION() (instrNo++)

static unsigned     pass            = 0;
static unsigned     instrNo         = 0;
static unsigned int variableCount   = 0;

static SymbolTable* table   = NULL;
static FILE*        outFile = NULL;

/* ------------------------ Predefined Symbols Variables ------------------------ */
typedef struct PredefinedSymbol {
    const char* name;
    int         address;
} PredefinedSymbol;

static const PredefinedSymbol predefinedSymbols[] = {
    {"R0",   0},  {"R1",  1},  {"R2",   2}, {"R3",   3},
    {"R4",   4},  {"R5",  5},  {"R6",   6}, {"R7",   7},
    {"R8",   8},  {"R9",  9},  {"R10", 10}, {"R11", 11},
    {"R12", 12},  {"R13", 13}, {"R14", 14}, {"R15", 15},
    {"SP",   0},  {"LCL",  1}, {"ARG",  2}, {"THIS", 3}, {"THAT", 4},
    {"SCREEN",  16384},
    {"KBD",     24576},
};

static const unsigned int predefinedSymbolsCount = sizeof(predefinedSymbols) / sizeof(predefinedSymbols[0]);

/* ------------------------ Code Mapping ------------------------ */
typedef struct CodeMapping {
    const char* mnemonic;
    const char* binary;
} CodeMapping;

static const CodeMapping compTable[] = {
    {"0",   "0101010"},
    {"1",   "0111111"},
    {"-1",  "0111010"},
    {"D",   "0001100"},
    {"A",   "0110000"}, {"M",   "1110000"},
    {"!D",  "0001101"},
    {"!A",  "0110001"}, {"!M",  "1110001"},
    {"-D",  "0001111"},
    {"-A",  "0110011"}, {"-M",  "1110011"},
    {"D+1", "0011111"},
    {"A+1", "0110111"}, {"M+1", "1110111"},
    {"D-1", "0001110"},
    {"A-1", "0110010"}, {"M-1", "1110010"},
    {"D+A", "0000010"}, {"D+M", "1000010"},
    {"D-A", "0010011"}, {"D-M", "1010011"},
    {"A-D", "0000111"}, {"M-D", "1000111"},
    {"D&A", "0000000"}, {"D&M", "1000000"},
    {"D|A", "0010101"}, {"D|M", "1010101"},
};

static const CodeMapping destTable[] = {
    {"M", "001"},
    {"D", "010"},
    {"DM", "011"},
    {"A", "100"},
    {"AM", "101"},
    {"AD", "110"},
    {"ADM", "111"}
};

static const CodeMapping jumpTable[] = {
    {"JGT", "001"},
    {"JEQ", "010"},
    {"JGE", "011"},
    {"JLT", "100"},
    {"JNE", "101"},
    {"JLE", "110"},
    {"JMP", "111"}
};

/* ------------------------ Static Declarations ------------------------ */
static void
installPredefinedSymbols();

static void
handle_A_command_int(int n);

static void
handle_A_command_id(char* id);

static void
handle_C_command_type1(char* comp, char* dest, char* jump);

static void
handle_C_command_type2(char* comp, char* dest);

static void
handle_C_command_type3(char* comp, char* jump);

static void
handle_L_command(char* id, unsigned int line);

static void
validateAInstructionValue(int value);

static const char*
lookupComp(char* mnemonic);

static const char*
lookupDest(char* mnemonic);

static const char*
lookupJump(char* mnemonic);

static void
build_C_instruction(const char* comp, const char* dest, const char* jump, char* output);

static void
convertAndWriteBinary(int n);

static char*
convertIntegerToBinary(int n);

static void
writeWord(char* word);

/* ------------------------ Implementation ------------------------ */
void
parserUtil_initialize(const char* filename) {
    table = symtab_initialize();

    installPredefinedSymbols();
}

void
parserUtil_setOutputFile(const char* filename) {
    if (!(outFile = fopen(filename, "w"))) {
        printf("Error opening output file.\n");
        exit(1);
    }
}

void
parserUtil_setFirstPass() {
    pass = 1;
}

void
parserUtil_setSecondPass() {
    pass = 2;
}

void
parserUtil_handle_A_command_int(int n) {
    if (pass == 1) {
        INCREMENT_INSTRUCTION();
    }
    else {
        handle_A_command_int(n);
    }
}

void
parserUtil_handle_A_command_id(char* id) {
    if (pass == 1) {
        INCREMENT_INSTRUCTION();
    }
    else {
        handle_A_command_id(id);
    }
}

void
parserUtil_handle_C_command_type1(char* dest, char* comp, char* jump) {
    if (pass == 1) {
        INCREMENT_INSTRUCTION();
    }
    else {
        handle_C_command_type1(dest, comp, jump);
    }
}

void
parserUtil_handle_C_command_type2(char* comp, char* dest) {
    if (pass == 1) {
        INCREMENT_INSTRUCTION();
    }
    else {
        handle_C_command_type2(comp, dest);
    }
}

void
parserUtil_handle_C_command_type3(char* comp, char* jump) {
    if (pass == 1) {
        INCREMENT_INSTRUCTION();
    }
    else {
        handle_C_command_type3(comp, jump);
    }
}

void
parserUtil_handle_L_command(char* id, unsigned int line) {
    if (pass == 1) {
        handle_L_command(id, line);
    }
}

void
parserUtil_cleanup() {
    symtab_cleanup(table);
}

/* ------------------------ Static Definitions ------------------------ */
static void
installPredefinedSymbols() {
    for (int i = 0; i < predefinedSymbolsCount; i++) {
        symtab_insert(table, (char*)predefinedSymbols[i].name, predefinedSymbols[i].address);
    }
}

static void
handle_A_command_int(int n) {
    char* word;

    validateAInstructionValue(n);
    convertAndWriteBinary(n);
}

static void
handle_A_command_id(char* id) {
    if (symtab_exists(table, id)) {
        int value = symtab_getValue(table, id);
        convertAndWriteBinary(value);
    }
    else {
        unsigned int ramAddr = HACK_VARIABLE_BASE + variableCount++;
        symtab_insert(table, id, ramAddr);
        convertAndWriteBinary(ramAddr);
    }
}

static void
handle_C_command_type1(char* comp, char* dest, char* jump) {
    const char* compBinary = lookupComp(comp);
    const char* destBinary = lookupDest(dest);
    const char* jumpBinary = lookupJump(jump);

    char binary[17];
    build_C_instruction(compBinary, destBinary, jumpBinary, binary);
    writeWord(binary);
}

static void
handle_C_command_type2(char* comp, char* dest) {
    const char* compBinary = lookupComp(comp);
    const char* destBinary = lookupDest(dest);

    char binary[17];
    build_C_instruction(compBinary, destBinary, "000", binary);
    writeWord(binary);
}

static void
handle_C_command_type3(char* comp, char* jump) {
    const char* compBinary = lookupComp(comp);
    const char* jumpBinary = lookupJump(jump);

    char binary[17];
    build_C_instruction(compBinary, "000", jumpBinary, binary);
    writeWord(binary);
}

static void
handle_L_command(char* id, unsigned int line) {
    if (symtab_exists(table, id)) {
        printf("Label \"%s\" on line %d is already defined.\n", id, line);
        exit(1);
    }
    
    symtab_insert(table, id, instrNo);
}

static void
validateAInstructionValue(int value) {
    if (value < 0 || value > A_INSTRUCTION_MAX) {
        fprintf(stderr, "Error: A-instruction value %d is out of range (0-%d).\n", value, A_INSTRUCTION_MAX);
        exit(1);
    }
}

static const char*
lookupComp(char* mnemonic) {
    int size = sizeof(compTable) / sizeof(compTable[0]);

    for (int i = 0; i < size; i++) {
        if (strcmp(compTable[i].mnemonic, mnemonic) == 0) {
            return compTable[i].binary;
        }
    }

    printf("Error: Mnemonic \"%s\" for computation not found.\n", mnemonic);
    exit(1);
}

static const char*
lookupDest(char* mnemonic) {
    int size = sizeof(destTable) / sizeof(destTable[0]);

    for (int i = 0; i < size; i++) {
        if (strcmp(destTable[i].mnemonic, mnemonic) == 0) {
            return destTable[i].binary;
        }
    }

    printf("Error: Mnemonic \"%s\" for destination not found.\n", mnemonic);
    exit(1);
}

static const char*
lookupJump(char* mnemonic) {
    int size = sizeof(jumpTable) / sizeof(jumpTable[0]);

    for (int i = 0; i < size; i++) {
        if (strcmp(jumpTable[i].mnemonic, mnemonic) == 0) {
            return jumpTable[i].binary;
        }
    }

    printf("Error: Mnemonic \"%s\" for jump not found.\n", mnemonic);
    exit(1);
}

static void
build_C_instruction(const char* comp, const char* dest, const char* jump, char* output) {
    snprintf(output, WORD_SIZE + 1, "111%s%s%s", comp, dest, jump);
}

static void
convertAndWriteBinary(int n) {
    char* word;

    word = convertIntegerToBinary(n);
    writeWord(word);

    free(word);
}

static char*
convertIntegerToBinary(int n) {
    char* word;

    word = malloc(sizeof(char) * (WORD_SIZE + 1));
    if (!word) {
        printf("Error allocating memory word for A instruction.\n");
        exit(1);
    }

    for (int bit = 0; bit < WORD_SIZE; bit++) {
        word[WORD_SIZE - bit - 1] = (n & (1 << bit)) ? '1' : '0';
    }
    word[WORD_SIZE] = '\0';

    return word;
}

static void
writeWord(char* word) {
    fprintf(outFile, "%s\n", word);
}