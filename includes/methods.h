#ifndef METHODS_H
#define METHODS_H

#include "utils.h"
#include "types.h"
#include "pool.h"

int method_post(char *sv_name, struct TODO *task);
void method_options(char *sv_name, struct TODO *task);
void method_get(char *sv_name, struct TODO *task);

#endif