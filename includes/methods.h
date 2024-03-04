#ifndef METHODS_H
#define METHODS_H

#include "utils.h"
#include "types.h"
#include "pool.h"

int method_post(struct ServerConfig config, struct TODO *task);
void method_options(char *sv_name, struct TODO *task);
int method_get(struct ServerConfig config, struct TODO *task);

#endif