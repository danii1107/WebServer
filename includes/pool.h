#ifndef POOL_H
#define POOL_H

#define MAX_THREADS 100
#define QUEUE_SIZE 100

# include <signal.h>

// Estructura de la tarea
struct TODO {
    int client_sock;    			// Descriptor de socket para comunicarsse con el cliente
    int len;           				// Longitud de la solicitud
    char verb[8];       			// Método HTTP: "GET", "POST", "OPTIONS"
    char uri[1024];     			// URI solicitada 
    char version[16];   			// Versión del protocolo
    char post_data[1024];			// Datos POST
};

// Estructura del pool de hilos
struct Pool {
    pthread_t threads[MAX_THREADS];  // Hilos
    struct TODO todo_q[QUEUE_SIZE];  // COla de tareas
    int q_size;                      // Tamaño de la cola de tareas
    pthread_mutex_t lock;            // Mutex para la cola dew tareas
    pthread_cond_t cond;             // Detectan la cola vacia y no lamnza hilos
    volatile sig_atomic_t shutdown;  // Flag para terminar hilos
};

void initialize_thread_pool(struct Pool *pool);

#endif