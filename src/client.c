#include "../includes/types.h"
#include "../includes/sockets.h"
#include "netinet/in.h"

static volatile sig_atomic_t got_sigint = 0;

// Control señal sigint, cerramos conexión y liberamos recursos
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

    // Crear y establecer conexiones del socket
    sock = make_connection_client(&server_address);
    if (sock < 0)
    {
        // free
        exit(EXIT_FAILURE);
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

        // Lee el mensaje recibido del servidor
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

    // Si cerramos socket informamos al servidor para que cierre la conexión
    sent_bytes = send(sock, "exit", 5, 0);
    if (sent_bytes < 0) {
        perror("Error al enviar el mensaje");
        exit(EXIT_FAILURE);
    }
    // Cerrar el socket antes de terminar el programa
    close(sock);

    return 0;
}
