#include "../includes/methods.h"

void  method_get(char *sv_name, struct TODO *task) {
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
                         "%s",
                         task->version, date, sv_name, strlen(script_output), script_output);
        } else { // Si no es un script, armar respuesta por defecto
            sprintf(http_response, "%s 200 OK\r\n"
                         "Date: %s\r\n"
                         "Server: %s\r\n"
                         "Content-Type: text/plain; charset=UTF-8\r\n"
                         "Content-Length: %lu\r\n"
                         "Connection: close\r\n"
                         "\r\n"
                         "%s",
                         task->version, date, sv_name, (unsigned long)strlen(task->data), task->data);
        }
    } else {
        // Si no hay datos, armar respuesta por defecto
        snprintf(http_response, sizeof(http_response),
             "%s 200 OK\r\n"
             "Date: %s\n"
             "Server: %s\n"
             "Content-Type: text/html; charset=UTF-8\r\n"
             "Content-Length: 0\r\n"
             "Connection: close\r\n"
             "\r\n", 
             task->version, date, sv_name);
    }
    // Enviar respuesta
    send(task->client_sock, http_response, strlen(http_response), 0);
}

int method_post(char *sv_name, struct TODO *task)
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
                          "%s",
                          task->version, date, sv_name, strlen(script_output), script_output);
        } else { // Si no es un script, armar respuesta por defecto
            sprintf(http_response, "%s 200 OK\r\n"
                          "Date: %s\r\n"
                          "Server: %s\r\n"
                          "Content-Type: text/plain; charset=UTF-8\r\n"
                          "Content-Length: %lu\r\n"
                          "Connection: close\r\n"
                          "\r\n"
                          "Datos recibidos correctamente.",
                          task->version, date, sv_name, (unsigned long)strlen("Datos recibidos correctamente."));
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
					 "Content-Length: %ld\n"
					 "Content-Type: text/plain\n" 
                     "Connection: close\r\n"
					 "\n", task->version, sv_name, date, strlen(response));
	send(task->client_sock, response, strlen(response), 0);
}