#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

typedef struct {
    char*       output_folder_name;
    FILE*       output_file;
    char*       input_filename;
    unsigned    instruction_count;
    char*       current_function_name;
    unsigned    return_address_count;
} VMContext;

extern VMContext vm_context;

#define OUT_FOLDER  (vm_context.output_folder_name)
#define OUT_FILE    (vm_context.output_file)

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
    ADD_CMD,
    SUB_CMD,
    NEG_CMD,
    EQ_CMD,
    GT_CMD,
    LT_CMD,
    AND_CMD,
    OR_CMD,
    NOT_CMD
} ComputeOp_T;

typedef enum {
    MEM_POP,
    MEM_PUSH
} MemOp_T;

typedef enum {
    SIMPLE_LABEL,
    GOTO_LABEL,
    IFGOTO_LABEL
} LabelOp_T;

#endif