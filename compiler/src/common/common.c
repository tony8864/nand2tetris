#include "common.h"

unsigned lex_debug_mode = 0;
unsigned bison_debug_mode = 0;

void
common_set_lex_debug_mode(unsigned enabled) {
    lex_debug_mode = enabled;
}

void
common_set_bison_debug_mode(unsigned enabled) {
    bison_debug_mode = enabled;
}