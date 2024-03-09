/**
 * @file utils.h
 * @author Daniel Birsan y Diego Rodrigo
 */

#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include "pool.h"
#include "log.h"

/********
* FUNCIÓN: int get_verb(const char* verb)
* ARGS_IN: const char* verb - Verbo HTTP como cadena de caracteres ("GET", "POST", "OPTIONS").
* DESCRIPCIÓN: Comprueba el verbo HTTP proporcionado y devuelve un entero asociado a ese verbo.
* ARGS_OUT: int - Devuelve 0 para "GET", 1 para "POST", 2 para "OPTIONS", y -1 si el verbo no es reconocido.
********/
int get_verb(const char *verb);

/********
* FUNCIÓN: void get_date(char *date)
* ARGS_IN: char *date - Puntero a una cadena de caracteres donde almacenar la fecha actual.
* DESCRIPCIÓN: Obtiene la fecha y hora actuales en formato GMT y las escribe en la cadena de caracteres apuntada por date.
* ARGS_OUT: Ninguno (void). La fecha y hora se almacenan en la cadena de caracteres apuntada por date.
********/
void get_date(char *date);

/********
* FUNCIÓN: int get_content_type(char *uri, char *ct)
* ARGS_IN: char *uri - URI del recurso solicitado, char *ct - Puntero a una cadena de caracteres donde almacenar el tipo de contenido.
* DESCRIPCIÓN: Determina el tipo de contenido (MIME type) del recurso solicitado basándose en su extensión de archivo y lo almacena en ct.
* ARGS_OUT: OK si el tipo de contenido se ha determinado con éxito, ERROR si no se ha podido determinar.
********/
STATUS get_content_type(char *uri, char *ct);

/********
* FUNCIÓN: int read_file(const char *path, char *buffer, size_t buffer_size)
* ARGS_IN: const char *path - Ruta al archivo a leer, char *buffer - Buffer donde almacenar el contenido del archivo, size_t buffer_size - Tamaño del buffer.
* DESCRIPCIÓN: Lee el contenido de un archivo y lo almacena en un buffer. El número de bytes leídos no superará buffer_size.
* ARGS_OUT: int - Devuelve el número de bytes leídos en caso de éxito, -1 si el archivo no puede ser abierto.
********/
int read_file(const char *path, char *buffer, size_t buffer_size);

/********
* FUNCIÓN: int execute_script(char *script_path, char *data[], char **response, ssize_t *response_size)
* ARGS_IN: char *script_path - Ruta al script a ejecutar, char *data[] - Array de cadenas con los argumentos para el script, char **response - Puntero a una cadena donde almacenar la salida del script, ssize_t *response_size - Puntero a una variable donde almacenar el tamaño de la respuesta.
* DESCRIPCIÓN: Ejecuta un script externo (Python o PHP), pasando los argumentos proporcionados y capturando su salida.
* ARGS_OUT: OK si el script se ha ejecutado con éxito, ERROR si se ha producido un error.
********/
STATUS execute_script(char *script_path, char *data[], char **response, ssize_t *response_size);

/********
* FUNCIÓN: void parse_args(const char *args, char *parsed_args[], size_t parsed_args_size)
* ARGS_IN: const char *args - Cadena de caracteres con los argumentos a parsear, char *parsed_args[] - Array de cadenas donde almacenar los argumentos parseados, size_t parsed_args_size - Tamaño del array parsed_args.
* DESCRIPCIÓN: Tokeniza una cadena de argumentos delimitados por '&' y almacena cada argumento en un array de cadenas.
* ARGS_OUT: OK si todo ha ido bien, ERROR si se ha producido un error.
********/
STATUS parse_args(const char *args, char *parsed_args[], size_t parsed_args_size);

#endif