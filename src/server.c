# include "../includes/sockets.h"
# include "../includes/types.h"
#include <netinet/in.h>

static volatile sig_atomic_t got_sigint = 0;
void **allocated_memory;

void handler_sigint(int sig)
{
    got_sigint = 1;
}

int main() {
    struct sockaddr_in address;
    struct sigaction act;
    ssize_t bytesRead, sent_bytes;
    pid_t pid;
    int addrlen = 0;
    int status, server_fd, new_socket;
    char buffer[BUFFER_SIZE] = {0};
    char t[BUFFER_SIZE] = "\0";

    // Reserva de memoria para la lista de direcciones de memoria
    /* allocated_memory = (void**) malloc(sizeof(void*));
    if (!allocated_memory)
        exit (EXIT_FAILURE); */

    // Crear y conectar socket
    server_fd = make_connection(&address);
    if (server_fd < 0)
    {
        //free(allocated_memory);
        exit(EXIT_FAILURE);
    }
    addrlen = sizeof(address);

    // Control señal sigint
    act.sa_handler = handler_sigint;
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    while (!got_sigint)
    {
        // Aceptar una conexión
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            if (got_sigint)
            {
                // esprar hijos
                break;
            }
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Leer datos del socket y escribirlos en el archivo
            while(1)
            {
                bytesRead = recvfrom(new_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&address, (socklen_t*)&addrlen);
                if (bytesRead < 0)
                {
                    perror("recvfrom");
                    exit(EXIT_FAILURE);
                }
                if (strcmp(buffer, "exit") == 0)
                {
                    close(new_socket);
                    write(1, "connection closed\n", 18);
                    exit(EXIT_SUCCESS);
                }
                write(1, buffer, strlen(buffer));
                fflush(stdout);
                memset((void*) buffer, 0, sizeof(buffer));
                
                bytesRead = read(0, buffer, BUFFER_SIZE);
                if (bytesRead < 0)
                {
                    perror("Error al recibir el mensaje STDIN");
                    exit(EXIT_FAILURE);
                }

                sent_bytes = send(new_socket, buffer, strlen(buffer), 0);
                if (sent_bytes < 0) {
                    close(new_socket);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    
    //frees
    close(server_fd);
    return 0;
}