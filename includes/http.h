/**
 * @file http.h
 * @author Daniel Birsan y Diego Rodrigo
 */

#ifndef HTTP_H
#define HTTP_H

#include "types.h"
#include "utils.h"
#include "picohttpparser.h"
#include "methods.h"
#include "pool.h"

/********
 * FUNCIÓN: int parse_http_request(const char* buffer, size_t buflen, struct TODO* task)
 * ARGS_IN: const char* buffer - buffer que contiene la petición HTTP,
 *          size_t buflen - longitud del buffer,
 *          struct TODO* task - estructura de tarea donde almacenar la petición parseada.
 * DESCRIPCIÓN: Parsea la petición HTTP del buffer y almacena la información relevante en la estructura de tarea.
 * ARGS_OUT: int - devuelve 1 en caso de éxito, 0 si la petición es incompleta, o -1 si hay un error.
 ********/
int parse_http_request(const char* buffer, size_t buflen, struct TODO* todo);

/********
* FUNCIÓN: void send_http_response(struct TODO *task, struct ServerConfig config)
* ARGS_IN: struct TODO *task - estructura de tarea con información sobre la petición,
*          struct ServerConfig config - configuración del servidor.
* DESCRIPCIÓN: Envía una respuesta HTTP al cliente basada en el verbo HTTP y la URI solicitada.
*              Comprueba si la URI intenta acceder a directorios fuera del root y, en caso afirmativo, envía un 403.
*              Si la URI es root, devuelve index.html. Concatena el root y la URI para obtener la ruta completa.
*              Luego, en función del verbo HTTP, llama a la función correspondiente para manejar la petición.
* ARGS_OUT: Ninguno (void).
********/
void send_http_response(struct TODO *task, struct ServerConfig config);

#endif