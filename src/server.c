/**
 * @brief Implementa la funcionalidad del servidor
 * @file server.c
 * @author Daniel Birsan y Diego Rodrigo
 * @version 1.0
 * @date 13/03/2024
 */

#include "../includes/http.h"
#include "../includes/types.h"
#include "../includes/pool.h"
#include "../includes/sockets.h"
#include "../includes/log.h"

static volatile sig_atomic_t got_sigint = 0;
static struct Pool *pool_ptr = NULL;

/********
 * FUNCIÓN: void handler_sigint()
 * ARGS_IN: Ninguno.
 * DESCRIPCIÓN: Manejador de la señal SIGINT (Ctrl+C). Establece la bandera got_sigint a 1
 *              e indica a los hilos del pool que deben terminar su ejecución marcando
 *              pool_ptr->shutdown a 1 y despertando a todos los hilos en espera con pthread_cond_broadcast.
 * ARGS_OUT: Ninguno (void).
 ********/
void handler_sigint()
{
    got_sigint = 1;
    if (pool_ptr != NULL)
    {
        pool_ptr->shutdown = 1;                    // Indica a los hilos que deben terminar
        pthread_cond_broadcast(&(pool_ptr->cond)); // Despierta a todos los hilos
    }
}

/********
 * FUNCIÓN: int read_server_config(struct ServerConfig *config)
 * ARGS_IN: struct ServerConfig *config - Puntero a la estructura donde se almacenará la configuración leída.
 * DESCRIPCIÓN: Lee la configuración del servidor desde el archivo "server.conf" y almacena los valores
 *              en la estructura ServerConfig proporcionada. Los valores de configuración incluyen
 *              puerto de escucha, número máximo de clientes, firma del servidor y directorio raíz.
 * ARGS_OUT: int - Devuelve 0 en caso de éxito, -1 en caso de error al abrir el archivo de configuración.
 ********/
int read_server_config(struct ServerConfig *config)
{
    FILE *file = fopen("server.conf", "r");
    if (file == NULL)
    {
        perror("fopen");
        return -1;
    }

    char line[256];
    char *key, *value;
    while (fgets(line, sizeof(line), file))
    {
        key = strtok(line, "=");
        value = strtok(NULL, "=");
        if (strcmp(key, "listen_port") == 0)
        {
            config->port = atoi(value);
        }
        else if (strcmp(key, "max_clients") == 0)
        {
            config->max_clients = atoi(value);
        }
        else if (strcmp(key, "server_signature") == 0)
        {
            strcpy(config->sv_name, value);
        }
        else if (strcmp(key, "server_root") == 0)
        {
            strncpy(config->root, value, strlen(value) - 1); // Eliminar salto de línea
        }
    }
    fclose(file);
    return 0;
}

/********
 * FUNCIÓN: int main()
 * ARGS_IN: Ninguno.
 * DESCRIPCIÓN: Punto de entrada principal del programa del servidor. Inicializa la configuración del servidor,
 *              establece el manejador de señal para SIGINT, crea un socket servidor, inicia el pool de hilos,
 *              y entra en un bucle donde acepta conexiones, recibe peticiones, y las agrega al pool de tareas para su procesamiento.
 *              Al recibir SIGINT, finaliza todos los hilos, libera recursos y cierra el servidor.
 * ARGS_OUT: Ninguno directamente, pero termina el programa con exit(EXIT_SUCCESS) o exit(EXIT_FAILURE).
 ********/
int main()
{
    struct Pool pool = {0};
    struct TODO task = {0};
    struct ServerConfig config = {0};
    struct sockaddr_in address = {0};
    struct sigaction act = {0};
    int addrlen = 0;
    int server_fd = 0, new_socket = 0;
    int pret = 0;
    int bytesRead = 0;
    char buffer[BUFFER_SIZE] = {0};

    // Asignar el puntero global a la estructura pool para controlar sigint
    pool_ptr = &pool;

    // Inicializar el log
    config.logFile = startLog("server.log");
    
    // Leer configuración del servidor
    if (read_server_config(&config) < 0)
    {
        writeToLog(config.logFile, "ERROR", "No se pudo leer el archivo de configuración", NULL);  
        exit(EXIT_FAILURE);
    }

    // Validar configuración
    if (config.port <= 1023 || config.max_clients <= 0 || strcmp(config.sv_name, "") == 0 || strcmp(config.root, "root") != 0)
    {
        writeToLog(config.logFile, "ERROR", "Configuración inválida", NULL);
        exit(EXIT_FAILURE);
    }

    // Crear y conectar socket
    server_fd = make_connection(&address, config);
    if (server_fd < 0)
    {
        writeToLog(config.logFile, "ERROR", "No se pudo crear el socket", NULL);
        exit(EXIT_FAILURE);
    }
    addrlen = sizeof(address);

    // Control señal sigint
    act.sa_handler = handler_sigint;
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;
    if (sigaction(SIGINT, &act, NULL) < 0)
    {
        writeToLog(config.logFile, "ERROR", "No se pudo establecer el manejador de señal", NULL);
        close(server_fd);
        stopLog(config.logFile);
        exit(EXIT_FAILURE);
    }

    // LLamar modulo hilos
    pool.config = config;
    pool.threads = (pthread_t *)malloc(config.max_clients * sizeof(pthread_t));
    if (!pool.threads)
    {
        writeToLog(config.logFile, "ERROR", "No se pudo reservar memoria para los hilos", NULL);
        close(server_fd);
        stopLog(config.logFile);
        exit(EXIT_FAILURE);
    }

    // Crear el pool de hilos
    if (initialize_thread_pool(&pool) != OK)
    {
        writeToLog(config.logFile, "ERROR", "No se pudo inicializar el pool de hilos", NULL);
        close(server_fd);
        free(pool.threads);
        stopLog(config.logFile);
        exit(EXIT_FAILURE);
    }

    // intentar socket varias peticiones en lugar de socket por peticion
    while (!got_sigint)
    {
        // Aceptar una conexión
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0)
        {
            if (got_sigint)
                break;
            perror("accept");
            continue; // NO TIRAR EL SERVIDOR
        }

        // Recibir petición
        bytesRead = recv(new_socket, buffer, BUFFER_SIZE, 0);
        if (bytesRead <= 0)
        {
            if (got_sigint)
                break;
            perror("recv");
            continue; // NO TIAR EL SERVIDOR
        }

        // Preparar la tarea
        task.client_sock = new_socket;
        pret = parse_http_request(buffer, bytesRead, &task, config.logFile);

        // Agregar tarea al pool
        if (pret > 0)
        {
            pthread_mutex_lock(&(pool.lock));   // Acceso zona crítica
            pool.todo_q[pool.q_size++] = task;  // 'Push' cola de tareas
            pthread_cond_signal(&(pool.cond));  // 'Flag' nueva task
            pthread_mutex_unlock(&(pool.lock)); // Abrir mutex
        }

        // Limpiar buffer para la próxima petición
        memset((void *)buffer, 0, BUFFER_SIZE);
    }

    // Cancelar todos los hilos y liberar recursos
    stop_thread_pool(&pool);
    stopLog(config.logFile);
    writeToLog(config.logFile, "INFO", "Servidor finalizado", NULL);
    free(pool.threads);
    close(server_fd);
    exit(EXIT_SUCCESS);
}