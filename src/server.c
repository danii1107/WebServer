# include "../includes/tcp.h"

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int status;
    char buffer[BUFFER_SIZE] = {0};
    char t[BUFFER_SIZE] = "\0";
    pid_t pid;
    ssize_t bytesRead;

    // Crear descriptor de archivo para el socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Adjuntar el socket al puerto 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular el socket al puerto 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Poner el servidor a escuchar conexiones
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // Aceptar una conexión
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            sprintf(t, "%d", getpid());
            // Abrir un archivo para guardar los datos recibidos
            FILE *file = fopen(t, "wb");
            if (file == NULL) {
                perror("Error al abrir el archivo");
                exit(EXIT_FAILURE);
            }
            // Leer datos del socket y escribirlos en el archivo
            while ((bytesRead = read(new_socket, buffer, BUFFER_SIZE)) > 0) {
                fwrite(buffer, 1, bytesRead, file);
            }

            if (bytesRead < 0) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            // Cerrar el archivo
            fclose(file);
            // Cerrar el socket 
            close(new_socket);
            exit(EXIT_SUCCESS);
        }
    }

    close(server_fd);
    return 0;
}