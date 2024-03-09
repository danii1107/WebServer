/**
 * @brief Implementa la funcionalidad del pool de hilos
 * @file pool.c
 * @author Daniel Birsan y Diego Rodrigo
 * @version 1.0
 * @date 13/03/2024
 */

#include "../includes/pool.h"
#include "../includes/http.h"

/********
 * FUNCIÓN: void *handle_client_request(void *aux, struct ServerConfig config)
 * ARGS_IN: void *aux - Puntero a la estructura TODO que contiene la tarea a procesar,
 *          struct ServerConfig config - Configuración actual del servidor.
 * DESCRIPCIÓN: Esta función maneja la solicitud del cliente recibida a través de la estructura TODO,
 *              envía una respuesta HTTP al cliente utilizando los datos de la solicitud y la configuración del servidor,
 *              y finalmente cierra el socket asociado al cliente.
 * ARGS_OUT: NULL - Retorna un puntero nulo, ya que esta función no produce un resultado directo más allá de su efecto de enviar la respuesta y cerrar el socket.
 ********/
void *handle_client_request(void *aux, struct ServerConfig config)
{
    struct TODO *task = (struct TODO *)aux;

    // ENviar respuesta al cliente
    send_http_response(task, config);

    // Cerrar el socket
    close(task->client_sock);
    return NULL;
}

/********
 * FUNCIÓN: void *thread_function(void *aux)
 * ARGS_IN: void *aux - Puntero a la estructura Pool que representa el pool de hilos y la cola de tareas.
 * DESCRIPCIÓN: Función ejecutada por cada hilo del pool. Controla la concurrencia mediante mutexes y espera condicional,
 *              toma tareas de la cola de tareas del pool, y llama a handle_client_request para procesarlas.
 *              Continúa procesando tareas hasta que se señala el cierre del servidor.
 * ARGS_OUT: NULL - Retorna un puntero nulo ya que el propósito principal es ejecutar tareas de forma continua.
 ********/
void *thread_function(void *aux)
{
    struct Pool *pool = (struct Pool *)aux;

    while (!pool->shutdown)
    {
        // Mutex control de concurrencia
        pthread_mutex_lock(&(pool->lock));
        while (pool->q_size == 0 && !pool->shutdown)
        {
            // Esperamos en vez de lanzar hjilos para mejorar eficiencia
            pthread_cond_wait(&(pool->cond), &(pool->lock));
        }
        if (pool->shutdown)
        {
            pthread_mutex_unlock(&(pool->lock));
            break; // Salir del bucle y terminar el hilo
        }

        // 'Popear' tarea de la cola
        struct TODO task = pool->todo_q[--pool->q_size];
        pthread_mutex_unlock(&(pool->lock));

        // Procesar la solicitud
        handle_client_request(&task, pool->config);
    }
    return NULL;
}

/********
 * FUNCIÓN: void initialize_thread_pool(struct Pool *pool)
 * ARGS_IN: struct Pool *pool - Puntero a la estructura Pool que se va a inicializar y donde se van a crear los hilos.
 * DESCRIPCIÓN: Inicializa el pool de hilos, configurando los mutex y variables de condición necesarios para la sincronización,
 *              establece el estado inicial de la cola de tareas y el indicador de cierre. Luego, crea los hilos que manejarán las solicitudes,
 *              asignándoles la función thread_function para su ejecución.
 * ARGS_OUT: ERROR si algo falla, OK si no lo hace.
 ********/
STATUS initialize_thread_pool(struct Pool *pool)
{
    if (pthread_mutex_init(&(pool->lock), NULL) != 0)
    {
        writeToLog(pool->config.logFile, "ERROR", "Error al inicializar el mutex\n", pool);
        return ERROR;
    }
    if (pthread_mutex_init(&(pool->logMutex), NULL) != 0)
    {
        writeToLog(pool->config.logFile, "ERROR", "Error al inicializar el mutex del log\n", pool);
        pthread_mutex_destroy(&(pool->lock));
        return ERROR;
    }
    if (pthread_cond_init(&(pool->cond), NULL) != 0)
    {
        writeToLog(pool->config.logFile, "ERROR", "Error al inicializar la variable de condición\n", pool);
        pthread_mutex_destroy(&(pool->lock));
        pthread_mutex_destroy(&(pool->logMutex));
        return ERROR;
    }
    pool->q_size = 0;
    pool->shutdown = 0;
    pool->active_threads = 0;
    for (int i = 0; i < pool->config.max_clients; i++)
    {
        int rc = pthread_create(&(pool->threads[i]), NULL, thread_function, pool);
        if (rc != 0)
        {
            writeToLog(pool->config.logFile, "WARNING", "Error al crear un hilo del pool\n", pool);
            break; // Si falla la creación de un hilo, se decide no abortar el proceso completo sino continuar con los hilos ya creados
        } else pool->active_threads++; // Incrementa el contador de hilos activos
    }

    // Si no se creó ningún hilo
    if (pool->active_threads == 0)
    {
        pthread_mutex_destroy(&(pool->lock));
        pthread_mutex_destroy(&(pool->logMutex));
        pthread_cond_destroy(&(pool->cond));
        writeToLog(pool->config.logFile, "ERROR", "No se pudo crear ningún hilo\n", pool);
        return ERROR;
    }

    if (pool->active_threads < pool->config.max_clients)
        writeToLog(pool->config.logFile, "WARNING", "No se pudieron crear todos los hilos del pool\n", pool);
    // Si se creó al menos un hilo, continua la ejecución normalmente
    return OK;
}

/********
 * FUNCIÓN: void stop_thread_pool(struct Pool* pool)
 * ARGS_IN: struct Pool *pool - Puntero a la estructura Pool
 * DESCRIPCIÓN: Detiene el pool de hilos, cancelando todos los hilos y liberando los recursos asociados.
 * ARGS_OUT: -
 ********/
void stop_thread_pool(struct Pool *pool)
{
    pthread_cond_broadcast(&(pool->cond));

    for (int i = 0; i < pool->active_threads; i++)
        pthread_join(pool->threads[i], NULL);
    
    pthread_mutex_destroy(&(pool->lock));
    pthread_mutex_destroy(&(pool->logMutex));
    pthread_cond_destroy(&(pool->cond));
}
