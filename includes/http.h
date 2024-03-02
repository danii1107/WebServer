#ifndef HTTP_H
#define HTTP_H

#include "../includes/types.h"
#include "../includes/pool.h"

int parse_http_request(int socket, const char* buffer, size_t buflen, struct TODO* todo);

#endif