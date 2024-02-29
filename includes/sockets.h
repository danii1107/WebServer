#ifndef SOCKETS_H
#define SOCKETS_H

#include <netinet/in.h>

int make_connection(struct sockaddr_in* address);
int make_connection_client(struct sockaddr_in* address);

#endif