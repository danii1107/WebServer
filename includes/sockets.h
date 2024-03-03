#ifndef SOCKETS_H
#define SOCKETS_H

#include <netinet/in.h>
#include "types.h"

int make_connection(struct sockaddr_in* address, struct ServerConfig config);

#endif