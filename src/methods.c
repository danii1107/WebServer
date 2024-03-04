#include "../includes/methods.h"

int  method_get(struct ServerConfig config, struct TODO *task) {
	char http_response[16384];
    char date[64];

    get_date(date);
    if (task->data[0]) {
        // Comprobar si es un script y ejecutarlo
        if (strstr(task->uri, ".py") || strstr(task->uri, ".php")) { 
            char script_output[4096];
            execute_script(task->uri, task->data, script_output, sizeof(script_output));
            // Armar respuesta script
            sprintf(http_response, "%s 200 OK\r\n"
                         "Date: %s\r\n"
                         "Server: %s\r\n"
                         "Content-Type: text/plain; charset=UTF-8\r\n"
                         "Content-Length: %lu\r\n"
                         "Connection: close\r\n"
                         "\r\n"
                         "%s\n",
                         task->version, date, config.sv_name, strlen(script_output), script_output);
        } else { // Si no es un script, armar respuesta por defecto
            sprintf(http_response, "%s 200 OK\r\n"
                         "Date: %s\r\n"
                         "Server: %s\r\n"
                         "Content-Type: text/html; charset=UTF-8\r\n"
                         "Content-Length: %lu\r\n"
                         "Connection: close\r\n"
                         "\r\n"
                         "%s\n",
                         task->version, date, config.sv_name, (unsigned long)strlen(task->data), task->data);
        }
        memset(task->data, 0, sizeof(task->data));
    } else { // Si no hay datos, buscar el archivo solicitado
        // Si se busca el root devolver index.html
        if (strcmp(task->uri, "/") == 0) {
            strcpy(task->uri, "index.html");
        } else { // Eliminar la barra inicial de uri
            memmove(task->uri, task->uri + 1, strlen(task->uri));
        }
        
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

        free(file_content);
        return 0;
    }
    // Enviar respuesta si no se ha enviado ya
    send(task->client_sock, http_response, strlen(http_response), 0);
    return 0;
}

int method_post(struct ServerConfig config, struct TODO *task)
{
    char http_response[5000];
    char date[64];

    get_date(date);
    if (task->data[0]) {
        // Comprobar si es un script y ejecutarlo
        if (strstr(task->uri, ".py") || strstr(task->uri, ".php")) {
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
        } else { // Si no es un script, armar respuesta por defecto
            sprintf(http_response, "%s 200 OK\r\n"
                          "Date: %s\r\n"
                          "Server: %s\r\n"
                          "Content-Type: text/plain; charset=UTF-8\r\n"
                          "Content-Length: %lu\r\n"
                          "Connection: close\r\n"
                          "\r\n"
                          "Datos recibidos correctamente.",
                          task->version, date, config.sv_name, (unsigned long)strlen("Datos recibidos correctamente."));
        }
        // Enviar respuesta
        send(task->client_sock, http_response, strlen(http_response), 0);
        return 0;
    }
    // Si no hay datos, enviar respuesta de error
    return -1;
}

void method_options(char *sv_name, struct TODO *task) {
	char response[256];     
	char date[64];

	get_date(date);
	sprintf(response, "%s 200 OK\n"
					 "Server: %s\n"
					 "Date: %s\n"
					 "Allow: GET, POST, OPTIONS\n"
					 "Content-Length: 0\n"
					 "Content-Type: text/plain\n" 
                     "Connection: close\r\n"
					 "\n", task->version, sv_name, date);
	send(task->client_sock, response, strlen(response), 0);
}