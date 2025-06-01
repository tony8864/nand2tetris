#ifndef PARSER_UTIL_H
#define PARSER_UTIL_H

void
parserUtil_initialize();

void
parserUtil_setOutputFile(const char* filename);

void
parserUtil_setFirstPass();

void
parserUtil_setSecondPass();

void
parserUtil_handle_A_command_int(int n);

void
parserUtil_handle_A_command_id(char* id);

void
parserUtil_handle_C_command_type1(char* comp, char* dest, char* jump);

void
parserUtil_handle_C_command_type2(char* comp, char* dest);

void
parserUtil_handle_C_command_type3(char* comp, char* jump);

void
parserUtil_handle_L_command(char* id, unsigned int line);

void
parserUtil_cleanup();

#endif