#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

extern unsigned debug_mode;

#define DEBUG_PRINT(fmt, ...) \
            do { if (debug_mode) fprintf(stderr, fmt, ##__VA_ARGS__); } while (0)


void
common_set_debug_mode(unsigned enabled);

#endif