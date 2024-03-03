#ifndef UTILS_H
#define UTILS_H

#include "types.h"

int get_verb(const char *verb);
void get_date(char *date);
void execute_script(const char *script_path, const char *data, char *response, size_t response_size);

#endif