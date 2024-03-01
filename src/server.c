# include "../includes/sockets.h"
# include "../includes/pool.h"
# include "../includes/types.h"
#include <netinet/in.h>

static volatile sig_atomic_t got_sigint = 0;
// 3er paso static struct Pool *pool_ptr = NULL;
//void **allocated_memory;

// Manejo señal SIGINT, paramos bucles, liberamos recursos y paramos el servidor
void handler_sigint(int sig)
{
    got_sigint = 1;
    /* 3er paso: if (pool_ptr != NULL) {
        pool_ptr->shutdown = 1; // Indica a los hilos que deben terminar
        pthread_cond_broadcast(&(pool_ptr->cond)); // Despierta a todos los hilos
    } */
}

int main() {
    // 3er paso struct Pool pool;
    // 3er paso struct TODO task;
    struct sockaddr_in address;
    struct sigaction act;
    int addrlen = 0;
    int server_fd, new_socket; // 3er paso , flag = 0;

    // Asignar el puntero global a la estructura pool para controlar sigint
    // 3er paso pool_ptr = &pool;

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

    // LLamar modulo hilos
    // 3er paso: initialize_thread_pool(&pool);

    int sent_bytes = 0;
    int bytesRead = 0;
    char buffer[BUFFER_SIZE] = {0};
    while (!got_sigint)
    {
        // Aceptar una conexión
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            if (got_sigint) break;
            perror("accept"); // Controla error pero no tira el serviodr
            //flag = 1;
            continue;
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            while (1)
            {
                bytesRead = recv(new_socket, buffer, BUFFER_SIZE, 0);
                if (bytesRead < 0)
                {
                    perror("recvfrom");
                    exit(EXIT_FAILURE);
                }
                // Si el cliente cierra la conexion break (encontrar forma buena de hacerlo)
                if (strcmp(buffer, "exit") == 0)
                {
                    close(new_socket);
                    write(1, "connection closed\n", 18);
                    break;
                }
                write(1, buffer, strlen(buffer));
                fflush(stdout);
                memset((void*) buffer, 0, sizeof(buffer));

                // ENviar respuesta al cliente
                sent_bytes = send(new_socket, "Mensaje recibido\n", 18, 0);
                if (sent_bytes < 0) {
                    close(new_socket);
                    break;
                }
                // Cerrar el socket 
            }
            close(new_socket);
            exit(EXIT_SUCCESS);
        }

        /* 3er paso: if (!flag) // Preparar tarea si solo si se ha aceptado la conexión correctamente
        {
            // Preparar la tarea
            task.client_sock = new_socket;
            // campos task http

            // Agregar tarea al pool
            pthread_mutex_lock(&(pool.lock)); // Acceso zona crítica
            pool.todo_q[pool.q_size++] = task; // 'Push' cola de tareas
            pthread_cond_signal(&(pool.cond)); // 'FLag' nueva task
            pthread_mutex_unlock(&(pool.lock)); // Abrir mutex
        }
        flag = 0; */
    }
    
    //frees
    // Cancelar todos los hilos y liberar recursos
    /* 3er paso: for (int i = 0; i < MAX_THREADS; i++) {
        pthread_cancel(pool.threads[i]);
        pthread_join(pool.threads[i], NULL);
    }
    pthread_mutex_destroy(&(pool.lock));
    pthread_cond_destroy(&(pool.cond));
    */
    close(server_fd);
    return 0;
}