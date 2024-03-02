#include "../includes/utils.h"
#include "../includes/http.h"

/* int code, const char* message, const char* content_type, const char* body, struct TODO* todo */
void send_http_response(int socket, struct TODO *task){
/*     int verb;

    verb = get_Verb(task->verb);    
    switch (verb)
    {
    case 0: // GET
        break;
    case 1: // POST
        break;
    case 2: // OPTIONS
        break;
    default:
        break;
    } */
    
    char http_body[2048]; 
    char http_response[4096];

    // Simplemente pruebas para ver navegador, implementar cverbos
    snprintf(http_body, sizeof(http_body),
             "<!DOCTYPE html>\n"
             "<html>\n"
             "<head>\n"
             "    <title>Detalles de la Petición</title>\n"
             "</head>\n"
             "<body>\n"
             "    <h1>Detalles de la Petición</h1>\n"
             "    <p><strong>Método HTTP:</strong> %s</p>\n"
             "    <p><strong>URI:</strong> %s</p>\n"
             "    <p><strong>Versión del Protocolo:</strong> %s</p>\n"
             "    <p><strong>Datos POST (si aplicable):</strong> %s</p>\n"
             "</body>\n"
             "</html>\n",
             task->verb, task->uri, task->version, task->post_data[0] ? task->post_data : "N/A");
    snprintf(http_response, sizeof(http_response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html; charset=UTF-8\r\n"
             "Content-Length: %lu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s", 
             strlen(http_body), http_body);

    send(socket, http_response, strlen(http_response), 0);
}

int parse_http_request(int socket, const char* buffer, size_t buflen, struct TODO* task) {
    const char *method, *path;
    int minor_version;
    struct phr_header headers[100];
    
	size_t method_len, path_len, num_headers = sizeof(headers) / sizeof(headers[0]);

    int pret = phr_parse_request(buffer, buflen, &method, &method_len, &path, &path_len, 
                                 &minor_version, headers, &num_headers, 0);

    if (pret > 0) {
        // Copia el método, URI y versión del protocolo a la estructura TO-DO
        strncpy(task->verb, method, method_len);
        task->verb[method_len] = '\0';

        strncpy(task->uri, path, path_len);
        task->uri[path_len] = '\0';

        sprintf(task->version, "HTTP/1.%d", minor_version);

        task->len = buflen;
        
        // Data post
        task->post_data[0] = '\0';
        
        send_http_response(socket, task);

        return 1; // Éxito
    }

    return pret; // Devuelve el resultado del parseo (0 para incompleto, -1 para error)
}