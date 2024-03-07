#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include "pool.h"

int get_verb(const char *verb);
void get_date(char *date);
int get_content_type(char *uri, char *ct);
int read_file(const char *path, char *buffer, size_t buffer_size);
int execute_script(char *script_path, char *data[], char **response, ssize_t *response_size);
void parse_args(const char *args, char *parsed_args[], size_t parsed_args_size);

#endif