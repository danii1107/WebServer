/**
 * @brief Implementa la conectividad de los sockets
 * @file sockets.c
 * @author Daniel Birsan y Diego Rodrigo
 * @version 1.0
 * @date 13/03/2024
 */

#include "../includes/sockets.h"

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
int make_connection(struct sockaddr_in *address, struct ServerConfig config)
{
    int server_fd;
    int opt = 1;

    // Crear descriptor de archivo para el socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        return -1;
    }

    // Adjuntar el socket al puerto
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        return -1;
    }

    // Establecer campos de la estructura
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(config.port);

    // Vincular el socket al puerto
    if (bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0)
    {
        perror("bind failed");
        return -1;
    }

    // Poner el servidor a escuchar conexiones
    if (listen(server_fd, 64) < 0)
    {
        perror("listen");
        return -1;
    }

    fprintf(stdout, "----Bienvenido al servidor----\n");
    fprintf(stdout, "----Para detener el servidor use Ctrl+C----\n");
    fprintf(stdout, "----Escuchando en [%s:%d]----\n\n", inet_ntoa(address->sin_addr), ntohs(address->sin_port));

    return server_fd;
}