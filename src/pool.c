#include "../includes/pool.h"
#include "../includes/http.h"

// Procesar tarea (http request)
void *handle_client_request(void *aux, struct ServerConfig config) {
    struct TODO *task = (struct TODO *)aux;

	// ENviar respuesta al cliente
	send_http_response(task, config);
	
	// Cerrar el socket
	close(task->client_sock);
    return NULL;
}

// Lanzar hilos a su respectiva tarea
void *thread_function(void *aux) {
    struct Pool *pool = (struct Pool *)aux;

    while (!pool->shutdown) {
		// Mutex control de concurrencia
        pthread_mutex_lock(&(pool->lock));
        while (pool->q_size == 0 && !pool->shutdown) {
			// Esperamos en vez de lanzar hjilos para mejorar eficiencia
            pthread_cond_wait(&(pool->cond), &(pool->lock));
        }
		if (pool->shutdown) {
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

// Inicializar y asignar hilos
void initialize_thread_pool(struct Pool *pool) {
    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->cond), NULL);
    pool->q_size = 0;
	pool->shutdown = 0;
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&(pool->threads[i]), NULL, thread_function, pool);
    }
}
