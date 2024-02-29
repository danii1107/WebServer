#include "../includes/types.h"
#include "../includes/pool.h"

// Procesar tarea (http request)
void *handle_client_request(void *aux) {
    struct TODO *task = (struct TODO *)aux;
	ssize_t bytesRead, sent_bytes;
	char buffer[BUFFER_SIZE] = {0};
	
	// Comunicacion servidor a cliente
	while(1)
	{
		// leer del socket cliente
		bytesRead = recv(task->client_sock, buffer, BUFFER_SIZE, 0);
		if (bytesRead < 0)
		{
			perror("recvfrom");
			exit(EXIT_FAILURE);
		}
		if (strcmp(buffer, "exit") == 0)
		{
			close(task->client_sock);
			write(1, "connection closed\n", 18);
			break;
		}
		write(1, buffer, strlen(buffer));
		fflush(stdout);
		memset((void*) buffer, 0, sizeof(buffer));
		
		// Leer por stdin respuesta del servidor
		bytesRead = read(0, buffer, BUFFER_SIZE);
		if (bytesRead < 0)
		{
			perror("Error al recibir el mensaje STDIN");
			close(task->client_sock);
			break;
		}

		// ENviar respuesta al cliente
		sent_bytes = send(task->client_sock, buffer, strlen(buffer), 0);
		if (sent_bytes < 0) {
			close(task->client_sock);
			break;
		}
	}

	close(task->client_sock);
    return NULL;
}

// Lanzar hilos a su respectiva tarea
void *thread_function(void *aux) {
    struct Pool *pool = (struct Pool *)aux;

    while (1) {
		// Mutex control de concurrencia
        pthread_mutex_lock(&(pool->lock));
        while (pool->q_size == 0) {
			// Esperamos en vez de lanzar hjilos para mejorar eficiencia
            pthread_cond_wait(&(pool->cond), &(pool->lock));
        }
        // 'Popear' tarea de la cola
        struct TODO task = pool->todo_q[--pool->q_size];
        pthread_mutex_unlock(&(pool->lock));
        
        // Procesar la solicitud
        handle_client_request(&task);
    }
    return NULL;
}

// Inicializar y asignar hilos
void initialize_thread_pool(struct Pool *pool) {
    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->cond), NULL);
    pool->q_size = 0;
    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_create(&(pool->threads[i]), NULL, thread_function, pool);
    }
}
