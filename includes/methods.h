/**
 * @file methods.h
 * @author Daniel Birsan y Diego Rodrigo
 */

#ifndef METHODS_H
#define METHODS_H

#include "utils.h"
#include "types.h"
#include "pool.h"

/********
* FUNCIÓN: int method_post(struct ServerConfig config, struct TODO *task)
* ARGS_IN: struct ServerConfig config - Configuración del servidor,
*          struct TODO *task - Tarea con información sobre la solicitud HTTP.
* DESCRIPCIÓN: Maneja solicitudes POST. 
* ARGS_OUT: OK si todo ha ido bien, ERROR si se ha producido un error.
********/
STATUS method_post(struct ServerConfig config, struct TODO *task);

/********
* FUNCIÓN: void method_options(char *sv_name, struct TODO *task)
* ARGS_IN: char *sv_name - Nombre del servidor,
*          struct TODO *task - Tarea con información sobre la solicitud HTTP.
* DESCRIPCIÓN: Maneja solicitudes OPTIONS, enviando una respuesta que lista los métodos HTTP permitidos (GET, POST, OPTIONS).
* ARGS_OUT: OK si todo ha ido bien, ERROR si se ha producido un error.
********/
STATUS method_options(char *sv_name, struct TODO *task);

/********
* FUNCIÓN: int method_get(struct ServerConfig config, struct TODO *task)
* ARGS_IN: struct ServerConfig config - Configuración del servidor,
*          struct TODO *task - Tarea con información sobre la solicitud HTTP.
* DESCRIPCIÓN: Maneja solicitudes GET, sirviendo archivos estáticos o ejecutando scripts (PHP, Python).
*              Si el URI incluye un script, lo ejecuta y devuelve su salida. Si es una solicitud de archivo,
*              intenta abrir y enviar el archivo solicitado. Si los datos son innecesarios o el archivo no existe,
*              puede retornar un error.
* ARGS_OUT: OK si todo ha ido bien, ERROR si se ha producido un error.
********/
STATUS method_get(struct ServerConfig config, struct TODO *task);

#endif