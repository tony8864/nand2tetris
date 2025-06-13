#include "common.h"

unsigned debug_mode = 0;

void
common_set_debug_mode(unsigned enabled) {
    debug_mode = enabled;
}