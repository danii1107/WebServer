/**
 * @file types.h
 * @author Daniel Birsan y Diego Rodrigo
 */

# ifndef TYPES_H
# define TYPES_H

#define BUFFER_SIZE 2048

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <arpa/inet.h>
#include "picohttpparser.h"

struct ServerConfig {
	int port;
	int max_clients;
	char root[256];
	char sv_name[256];
	FILE *logFile;
};

typedef enum Status {
	OK,
	ERROR
} STATUS;

#endif