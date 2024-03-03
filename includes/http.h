#ifndef HTTP_H
#define HTTP_H

#include "types.h"
#include "utils.h"
#include "picohttpparser.h"
#include "methods.h"
#include "pool.h"

int parse_http_request(const char* buffer, size_t buflen, struct TODO* todo);
void send_http_response(struct TODO *task, struct ServerConfig config);

#endif