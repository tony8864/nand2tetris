#ifndef COMMON_H
#define COMMON_H

typedef enum {
    SEG_LOCAL,
    SEG_ARGUMENT,
    SEG_THIS,
    SEG_THAT,
    SEG_POINTER,
    SEG_TEMP,
    SEG_CONSTANT,
    SEG_STATIC,
    SEG_INVALID
} Segment_T;

typedef enum {
    MEM_POP,
    MEM_PUSH
} MemOp_T;

#endif