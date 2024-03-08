/**
 * @brief Implementa la funcionalidad de los metodos (get,post,options)
 * @file methods.c
 * @author Daniel Birsan y Diego Rodrigo
 * @version 1.0
 * @date 13/03/2024
 */

#include "../includes/methods.h"

/********
* FUNCIÓN: int method_get(struct ServerConfig config, struct TODO *task)
* ARGS_IN: struct ServerConfig config - Configuración del servidor,
*          struct TODO *task - Tarea con información sobre la solicitud HTTP.
* DESCRIPCIÓN: Maneja solicitudes GET, sirviendo archivos estáticos o ejecutando scripts (PHP, Python).
*              Si el URI incluye un script, lo ejecuta y devuelve su salida. Si es una solicitud de archivo,
*              intenta abrir y enviar el archivo solicitado. Si los datos son innecesarios o el archivo no existe,
*              puede retornar un error.
* ARGS_OUT: int - Devuelve 0 en caso de éxito, -1 en caso de error al abrir el archivo o determinar el tipo de contenido.
********/
int  method_get(struct ServerConfig config, struct TODO *task) {
	char http_response[16384];
    char date[64];
    char *script_output = NULL;
    int unnecesary_args = 0, script = 0;
    
    // Comprobar si es un script
    if (strstr(task->uri, ".py") || strstr(task->uri, ".php")) script = 1;

    get_date(date);
    // Comprobar si es un script y ejecutarlo
    if (script == 1) { 
        ssize_t script_output_size = 0;
        size_t count = 0;

        // Reservar memoria para array de argumentos y tokenizarlos
        for (int i = 0; task->data[i]; i++) {
            if (task->data[i] == '&') count++;
        }
        char *parsed_args[count + 1]; // NULL al final
        parse_args(task->data, parsed_args, count + 1);
        execute_script(task->uri, parsed_args, &script_output, &script_output_size);

        // Armar respuesta script
        sprintf(http_response, "%s 200 OK\r\n"
                        "Date: %s\r\n"
                        "Server: %s\r\n"
                        "Content-Type: text/plain; charset=UTF-8\r\n"
                        "Content-Length: %lu\r\n"
                        "Connection: close\r\n"
                        "\r\n"
                        "%s\n",
                        task->version, date, config.sv_name, script_output_size, script_output);
    } else if (task->data[0]) unnecesary_args = 1; // No es script pero tiene args innecesarios

    if ((unnecesary_args == 1) || !(task->data[0])) { // Si no hay datos o son innecesarios, buscar el archivo solicitado
        char content_type[64];
        if (get_content_type(task->uri, content_type) != 0)
            return -1;

        // Abre el archivo solicitado en modo de lectura
        FILE *file = fopen(task->uri, "rb");
        if (!file) {
            // Enviar una respuesta 404 Not Found si el archivo no existe
            return -1;
        }
        
        // Buscar el tamaño del archivo
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        rewind(file);
        
        // Leer el contenido del archivo
        char *file_content = malloc(file_size);
        fread(file_content, file_size, 1, file);
        fclose(file);

        // Preparar la cabecera HTTP
        sprintf(http_response, "%s 200 OK\r\n"
                                    "Date: %s\n"
                                    "Server: %s\n"
                                    "Content-Type: %s; charset=UTF-8\r\n"
                                    "Content-Length: %ld\r\n"
                                    "Connection: close\r\n"
                                    "\r\n",
                                    task->version, date, config.sv_name, content_type, file_size);

        // Enviar la cabecera HTTP
        send(task->client_sock, http_response, strlen(http_response), 0);

        // Enviar el contenido del archivo
        send(task->client_sock, file_content, file_size, 0);

        if (script_output) free(script_output);
        if (file_content) free(file_content);
        return 0;
    }
    // Enviar respuesta si no se ha enviado ya
    send(task->client_sock, http_response, strlen(http_response), 0);
    return 0;
}

/********
* FUNCIÓN: int method_post(struct ServerConfig config, struct TODO *task)
* ARGS_IN: struct ServerConfig config - Configuración del servidor,
*          struct TODO *task - Tarea con información sobre la solicitud HTTP.
* DESCRIPCIÓN: Maneja solicitudes POST. 
* ARGS_OUT: int - Devuelve 0 en caso de éxito, -1 en caso de error al abrir el archivo o determinar el tipo de contenido.
********/
int method_post(struct ServerConfig config, struct TODO *task)
{
    char http_response[5000];
    char date[64];

    get_date(date);
    // Comprobar si es un script y ejecutarlo
    /* if (strstr(task->uri, ".py") || strstr(task->uri, ".php")) {
        char script_output[4096];
        execute_script(task->uri, task->data, script_output, sizeof(script_output));
        // Armar respuesta
        sprintf(http_response, "%s 200 OK\r\n"
                        "Date: %s\r\n"
                        "Server: %s\r\n"
                        "Content-Type: text/html; charset=UTF-8\r\n"
                        "Content-Length: %lu\r\n"
                        "Connection: close\r\n"
                        "\r\n"
                        "%s\n",
                        task->version, date, config.sv_name, strlen(script_output), script_output);
    } else { */ // Suponmos poder enviar un POST sin necesidad de ser un script, en servidores reales pueden ejecutarse estos post, 
            // en nuestro caso mandamos un mensaje de confirmación
        sprintf(http_response, "%s 200 OK\r\n"
                        "Date: %s\r\n"
                        "Server: %s\r\n"
                        "Content-Type: text/plain; charset=UTF-8\r\n"
                        "Content-Length: %lu\r\n"
                        "Connection: close\r\n"
                        "\r\n"
                        "Datos recibidos correctamente.",
                        task->version, date, config.sv_name, (unsigned long)strlen("Datos recibidos correctamente."));
    //}
    // Enviar respuesta
    send(task->client_sock, http_response, strlen(http_response), 0);
    return 0;
}

/********
* FUNCIÓN: void method_options(char *sv_name, struct TODO *task)
* ARGS_IN: char *sv_name - Nombre del servidor,
*          struct TODO *task - Tarea con información sobre la solicitud HTTP.
* DESCRIPCIÓN: Maneja solicitudes OPTIONS, enviando una respuesta que lista los métodos HTTP permitidos (GET, POST, OPTIONS).
* ARGS_OUT: Ninguno (void).
********/
void method_options(char *sv_name, struct TODO *task) {
	char response[256];     
	char date[64];

    // Imprimimos información de options
	get_date(date);
	sprintf(response, "%s 200 OK\n"
					 "Server: %s\n"
					 "Date: %s\n"
					 "Allow: GET, POST, OPTIONS\n"
					 "Content-Length: 0\n"
					 "Content-Type: text/plain\n" 
                     "Connection: close\r\n"
					 "\n", task->version, sv_name, date);
    // Enviar respuesta
	send(task->client_sock, response, strlen(response), 0);
}