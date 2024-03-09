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
 * ARGS_OUT: Ninguno (void).
 ********/
void initialize_thread_pool(struct Pool *pool)
{
    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->cond), NULL);
    pool->q_size = 0;
    pool->shutdown = 0;
    for (int i = 0; i < pool->config.max_clients; i++)
    {
        pthread_create(&(pool->threads[i]), NULL, thread_function, pool);
    }
}
