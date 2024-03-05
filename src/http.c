#include "../includes/http.h"
#include "../includes/pool.h"

// 400
void http_400(char *sv_name, struct TODO *task)
{
    char http_body[2048]; 
    char http_response[4096];
	char date[64];

    get_date(date);
    // Pagina de error 400
    sprintf(http_body, "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>400 Bad Request</title><style>body {font-family: Arial, sans-serif;text-align: center;padding: 50px;}h1 {font-size: 36px;margin-bottom: 20px;}p {font-size: 18px;margin-bottom: 20px;}</style></head><body><h1>400 Bad Request</h1><p>Your browser sent a request that this server could not understand.</p></body></html>");
    snprintf(http_response, sizeof(http_response),
             "%s 400 Bad request\r\n"
             "Server: %s\r\n"
             "Date: %s\r\n"
             "Content-Type: text/html; charset=UTF-8\r\n"
             "Content-Length: %lu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s", 
             task->version, sv_name, date, strlen(http_body), http_body);
    // Enviar respuesta
    send(task->client_sock, http_response, strlen(http_response), 0);
}

// 403
void http_403(char *sv_name, struct TODO *task)
{
    char http_body[2048]; 
    char http_response[4096];
    char date[64];

    get_date(date);
    // Página de error 403
    sprintf(http_body, "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>403 Forbidden</title><style>body {font-family: Arial, sans-serif;text-align: center;padding: 50px;}h1 {font-size: 36px;margin-bottom: 20px;}p {font-size: 18px;margin-bottom: 20px;}</style></head><body><h1>403 Forbidden</h1><p>You don't have permission to access this resource.</p></body></html>");
    snprintf(http_response, sizeof(http_response),
             "%s 403 Forbidden\r\n"
             "Server: %s\r\n"
             "Date: %s\r\n"
             "Content-Type: text/html; charset=UTF-8\r\n"
             "Content-Length: %lu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s", 
             task->version, sv_name, date, strlen(http_body), http_body);
    // Enviar respuesta
    send(task->client_sock, http_response, strlen(http_response), 0);
}

// 404
void http_404(char *sv_name, struct TODO *task)
{
    char http_body[2048]; 
    char http_response[4096];
	char date[64];

    get_date(date);
    // Página de error 404
    sprintf(http_body, "<!DOCTYPE html><html><head><meta charset=\"UTF-8\"><title>404 Not Found</title><style>body {font-family: Arial, sans-serif;text-align: center;padding: 50px;}h1 {font-size: 36px;margin-bottom: 20px;}p {font-size: 18px;margin-bottom: 20px;}</style></head><body><h1>404 Not Found</h1><p>The page you are looking for could not be found.</p></body></html>");
    snprintf(http_response, sizeof(http_response),
             "%s 404 Not found\r\n"
             "Server: %s\r\n"
             "Date: %s\r\n"
             "Content-Type: text/html; charset=UTF-8\r\n"
             "Content-Length: %lu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s", 
             task->version, sv_name, date, strlen(http_body), http_body);
    // Enviar respuesta
    send(task->client_sock, http_response, strlen(http_response), 0);
}

    int parse_http_request(const char* buffer, size_t buflen, struct TODO* task) {
        const char *method, *path;
        int minor_version;
        struct phr_header headers[100];
        
        size_t method_len, path_len, num_headers = sizeof(headers) / sizeof(headers[0]);

        // Parsea la solicitud HTTP
        int pret = phr_parse_request(buffer, buflen, &method, &method_len, &path, &path_len, 
                                    &minor_version, headers, &num_headers, 0);

        // Si la solicitud fue parseada exitosamente
        if (pret > 0) {
            // Copia el método, URI y versión del protocolo a la estructura TO-DO
            strncpy(task->verb, method, method_len);
            task->verb[method_len] = '\0';
            strncpy(task->uri, path, path_len);
            task->uri[path_len] = '\0';
            sprintf(task->version, "HTTP/1.%d", minor_version);

            // Tamaño de la solicitud
            task->len = buflen;
            
            // Argumentos de la solicitud
            if (strncmp(method, "GET", method_len) == 0) {
                // Para GET, buscamos '?' en el path.
                const char* args_start = strchr(path, '?');
                // Si hay argumentos
                if (args_start != NULL) {
                    args_start++;
                    // Copiar los argumentos de la solicitud a la estructura TO-DO
                    size_t args_len = path_len - (args_start - path);
                    if (args_len < sizeof(task->data)) { // SEGFAULT
                        strncpy(task->data, args_start, args_len);
                        task->data[args_len] = '\0';
                    }
                    // ELiminar ? y argumentos de la URI
                    task->uri[path_len - args_len - 1] = '\0';
                } else {
                    task->data[0] = '\0';
                }  
            } else if (strncmp(method, "POST", method_len) == 0) {
                // Para POST, buscamos el cuerpo del mensaje
                const char* body_start = buffer + pret;
                size_t body_len = buflen - pret;
                // Copiar el cuerpo del mensaje a la estructura TO-DO
                if (body_len < sizeof(task->data)) {
                    strncpy(task->data, body_start, body_len);
                    task->data[body_len] = '\0';
                } else {
                    task->data[0] = '\0';
                }
            }

            // Mostrar petición en consola del servidor
            fflush(stdout);
            write(1, "--------------------------------\n\n", 34);
            fflush(stdout);
            write(1, buffer, buflen);
            fflush(stdout);
            write(1, "--------------------------------\n", 33);
            fflush(stdout);

            return 1; // Éxito
        }

        return pret; // Devuelve el resultado del parseo (0 para incompleto, -1 para error)
    }

void send_http_response(struct TODO *task, struct ServerConfig config) {
    int verb;

    // Comprobar que la petición no intente acceder a directorios fuera de root
    if (strstr(task->uri, "..") != NULL) {
        http_403(config.sv_name, task);
        return;
    }
    // Si se busca el root devolver index.html
    if (strcmp(task->uri, "/") == 0) {
        strcpy(task->uri, "/templates/index.html");
    }
    // Concatenar root y task->uri
    char *uri = malloc(strlen(task->uri) + strlen(config.root) + 1);
    strncpy(uri, config.root, strlen(config.root));
    uri = strcat(uri, task->uri);
    strcpy(task->uri, uri);
    free(uri);
    verb = get_verb(task->verb);
    switch (verb)
    {
    case 0: // GET
        if (method_get(config, task) < 0) {
            http_404(config.sv_name, task);
        }
        break;
    case 1: // POST
        if (method_post(config, task) < 0) {
            http_400(config.sv_name, task);
        }
        break;
    case 2: // OPTIONS
        method_options(config.sv_name, task);
        break;
    default:
        http_400(config.sv_name, task);
        break;
    }
}