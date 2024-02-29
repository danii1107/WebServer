# include "../includes/sockets.h"
# include "../includes/pool.h"
# include "../includes/types.h"
#include <netinet/in.h>

static volatile sig_atomic_t got_sigint = 0;
//void **allocated_memory;

// Manejo señal SIGINT, paramos bucles, liberamos recursos y paramos el servidor
void handler_sigint(int sig)
{
    got_sigint = 1;
}

int main() {
    struct Pool pool;
    struct TODO task;
    struct sockaddr_in address;
    struct sigaction act;
    int addrlen = 0;
    int server_fd, new_socket, flag = 0;

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
    initialize_thread_pool(&pool);

    while (!got_sigint)
    {
        // Aceptar una conexión
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            if (got_sigint) break;
            perror("accept"); // Controla error pero no tira el serviodr
            flag = 1;
            continue;
        }

        if (!flag) // Preparar tarea si solo si se ha aceptado la conexión correctamente
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
        flag = 0;
    }
    
    //frees
    // Cancelar todos los hilos y liberar recursos
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_cancel(pool.threads[i]);
        pthread_join(pool.threads[i], NULL);
    }
    pthread_mutex_destroy(&(pool.lock));
    pthread_cond_destroy(&(pool.cond));

    close(server_fd);
    return 0;
}