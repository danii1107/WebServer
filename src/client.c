#include "../includes/tcp.h"

int main(int argc, char **argv) {
    struct sockaddr_in server_address;
    int sock = 0;
    ssize_t read_bytes, sent_bytes;
    char buffer[BUFFER_SIZE] = {0};

    // Crear el socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error al crear el socket \n");
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convertir direcciones IPv4 e IPv6 de texto a binario
    if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        printf("\nDirección IP inválida/ Dirección no soportada \n");
        return -1;
    }

    // Conectar al servidor
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        printf("\nConexión fallida \n");
        return -1;
    }

    // Envía el mensaje pasado como argumento
    sent_bytes = send(sock, argv[1], strlen(argv[1]), 0);
    if (sent_bytes < 0) {
        perror("Error al enviar el mensaje");
        return -1;
    }

    // Cerrar el socket antes de terminar el programa
    close(sock);

    return 0;
}
