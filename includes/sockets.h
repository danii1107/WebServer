/**
 * @file sockets.h
 * @author Daniel Birsan y Diego Rodrigo
 */
#ifndef SOCKETS_H
#define SOCKETS_H

#include <netinet/in.h>
#include "types.h"
#include "log.h"

/********
* FUNCIÓN: int make_connection(struct sockaddr_in* address, struct ServerConfig config)
* ARGS_IN: struct sockaddr_in* address - Puntero a la estructura de dirección que será configurada con la dirección IP y puerto para el servidor,
*          struct ServerConfig config - Estructura que contiene la configuración del servidor, incluyendo el puerto a utilizar.
* DESCRIPCIÓN: Crea y configura un socket para el servidor. Establece el socket para que reutilice la dirección IP y puerto si es necesario,
*              liga el socket al puerto especificado en la configuración del servidor, y pone el servidor en modo de escucha para aceptar conexiones entrantes.
*              La función configura la estructura de dirección con la familia de direcciones, la dirección IP (cualquiera en este caso),
*              y el puerto de escucha convertido a formato de red. Se imprime información relevante sobre el inicio del servidor.
* ARGS_OUT: int - Devuelve el descriptor de archivo del socket del servidor en caso de éxito, -1 en caso de error.
********/
int make_connection(struct sockaddr_in* address, struct ServerConfig config);

#endif