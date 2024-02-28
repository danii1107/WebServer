#include "../includes/types.h"
#include "netinet/in.h"

static volatile sig_atomic_t got_sigint = 0;

void handler_sigint(int sig)
{
    got_sigint = 1;
}

int main() {
    struct sockaddr_in server_address;
    struct sigaction act;
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

    act.sa_handler = handler_sigint;
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    while(!got_sigint)
    {
        // Envía el mensaje leido pro entrada
        read_bytes = read(0, buffer, BUFFER_SIZE);
        if (read_bytes < 0)
        {
            if (got_sigint == 0)
            {
                perror("Error al recibir el mensaje STDIN");
                exit(EXIT_FAILURE);
            }
            break;
        }

        sent_bytes = send(sock, buffer, strlen(buffer), 0);
        if (sent_bytes < 0) {
            perror("Error al enviar el mensaje");
            exit(EXIT_FAILURE);
        }
        memset((void*) buffer, 0, sizeof(buffer));

        read_bytes = recv(sock, buffer, BUFFER_SIZE, 0);
        if (read_bytes < 0) {
            if (got_sigint == 0)
            {
                perror("Error al recibir el mensaje");
                exit(EXIT_FAILURE);
            }
            break;         
        }
        write(1, buffer, strlen(buffer));
        fflush(stdout);
    }

    sent_bytes = send(sock, "exit", 5, 0);
    if (sent_bytes < 0) {
        perror("Error al enviar el mensaje");
        exit(EXIT_FAILURE);
    }
    // Cerrar el socket antes de terminar el programa
    close(sock);

    return 0;
}
