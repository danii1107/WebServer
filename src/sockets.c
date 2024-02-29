#include "../includes/sockets.h"
#include "../includes/types.h"

int make_connection(struct sockaddr_in* address)
{
	int server_fd;
	int opt = 1;

	// Crear descriptor de archivo para el socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        return -1;
    }

    // Adjuntar el socket al puerto 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        return -1;
    }
    // Establecer campos de la estructura
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);

    // Vincular el socket al puerto 8080
    if (bind(server_fd, (struct sockaddr *)address, sizeof(*address)) < 0) {
        perror("bind failed");
        return -1;
    }

    // Poner el servidor a escuchar conexiones
    if (listen(server_fd, 16) < 0) {
        perror("listen");
        return -1;
    }

	return server_fd;
}

int make_connection_client(struct sockaddr_in* server_address)
{
    int sock = 0;

    // Crear el socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error al crear el socket \n");
        return -1;
    }

    // Establecer campos de la estructura
    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(PORT);

    // Convertir direcciones IPv4 e IPv6 de texto a binario
    if(inet_pton(AF_INET, "127.0.0.1", &server_address->sin_addr) <= 0) {
        printf("\nDirección IP inválida/ Dirección no soportada \n");
        return -1;
    }

    // Conectar al servidor
    if (connect(sock, (struct sockaddr *)server_address, sizeof(*server_address)) < 0) {
        printf("\nConexión fallida \n");
        return -1;
    }

    return sock;
}