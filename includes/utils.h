#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include "pool.h"

int get_verb(const char *verb);
void get_date(char *date);
int get_content_type(char *uri, char *ct);
int read_file(const char *path, char *buffer, size_t buffer_size);
void execute_script(const char *script_path, const char *data, char *response, size_t response_size);

#endif