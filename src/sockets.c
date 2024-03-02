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
    if (listen(server_fd, 100) < 0) {
        perror("listen");
        return -1;
    }

    fprintf(stdout, "----Bienvenido al servidor----\n");
	fprintf(stdout, "----Para detener el servidor use Ctrl+C----\n");
	fprintf(stdout, "----Escuchando en [%s:%d]----\n", inet_ntoa(address->sin_addr), ntohs(address->sin_port));

	return server_fd;
}