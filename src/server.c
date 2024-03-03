#include "../includes/http.h"
#include "../includes/types.h"
#include "../includes/pool.h"
#include "../includes/sockets.h"

static volatile sig_atomic_t got_sigint = 0;
static struct Pool *pool_ptr = NULL;

// Manejo señal SIGINT, paramos bucles, liberamos recursos y paramos el servidor
void handler_sigint()
{
    got_sigint = 1;
    if (pool_ptr != NULL) {
        pool_ptr->shutdown = 1; // Indica a los hilos que deben terminar
        pthread_cond_broadcast(&(pool_ptr->cond)); // Despierta a todos los hilos
    }
}

// Leer server.conf y cargar configuración
int read_server_config(struct ServerConfig *config)
{
    FILE *file = fopen("server.conf", "r");
    if (file == NULL) {
        perror("fopen");
        return -1;
    }

    char line[256];
    char *key, *value;
    while (fgets(line, sizeof(line), file)) {
        key = strtok(line, "=");
        value = strtok(NULL, "=");
        if (strcmp(key, "listen_port") == 0) {
            config->port = atoi(value);
        } else if (strcmp(key, "max_clients") == 0) {
            config->max_clients = atoi(value);
        } else if (strcmp(key, "server_signature") == 0) {
            strcpy(config->sv_name, value);
        } else if (strcmp(key, "server_root") == 0) {
            strcpy(config->root, value);
        }
    }
    fclose(file);
    return 0;
}

int main() {
    struct Pool pool;
    struct TODO task;
    struct ServerConfig config;
    struct sockaddr_in address;
    struct sigaction act;
    int addrlen = 0;
    int server_fd, new_socket;
    int pret = 0;
    int bytesRead = 0;
    char buffer[BUFFER_SIZE] = {0};

    // Asignar el puntero global a la estructura pool para controlar sigint
    pool_ptr = &pool;

    // Leer configuración del servidor
    if (read_server_config(&config) < 0) {
        exit(EXIT_FAILURE);
    }

    // Crear y conectar socket
    server_fd = make_connection(&address, config);
    if (server_fd < 0)
    {
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
    pool.config = config;
    initialize_thread_pool(&pool);
    
    // intentar socket varias peticiones en lugar de socket por peticion
    while (!got_sigint)
    {
        // Aceptar una conexión
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            if (got_sigint) break;
            perror("accept");
            continue; // NO TIRAR EL SERVIDOR
        }

        // Recibir petición
        bytesRead = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0) {
            if (got_sigint) break;
            perror("recv");
            continue; // NO TIAR EL SERVIDOR
        }

        // Preparar la tarea
        task.client_sock = new_socket;
        pret = parse_http_request(buffer, bytesRead, &task);

        // Agregar tarea al pool
        if (pret > 0) {
            pthread_mutex_lock(&(pool.lock)); // Acceso zona crítica
            pool.todo_q[pool.q_size++] = task; // 'Push' cola de tareas
            pthread_cond_signal(&(pool.cond)); // 'Flag' nueva task
            pthread_mutex_unlock(&(pool.lock)); // Abrir mutex
        }

        // Limpiar buffer para la próxima petición
        memset((void*) buffer, 0, BUFFER_SIZE);
    }

    // Cancelar todos los hilos y liberar recursos
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_cancel(pool.threads[i]);
        pthread_join(pool.threads[i], NULL);
    }
    pthread_mutex_destroy(&(pool.lock));
    pthread_cond_destroy(&(pool.cond));
    
    close(server_fd);
    exit(EXIT_SUCCESS);
}