/**
 * @brief Implementa la funcionalidad de HTTP
 * @file http.c
 * @author Daniel Birsan y Diego Rodrigo
 * @version 1.0
 * @date 13/03/2024
 */

#include "../includes/http.h"
#include "../includes/pool.h"
#include "../includes/log.h"

/********
 * FUNCIÓN: void http_400(char *sv_name, struct TODO *task)
 * ARGS_IN: char *sv_name - nombre del servidor,
 *          struct TODO *task - estructura de tarea con información sobre la petición.
 * DESCRIPCIÓN: Envía una respuesta HTTP 400 Bad Request al cliente.
 * ARGS_OUT: Ninguno (void).
 ********/
void http_400(char *sv_name, struct TODO *task)
{
    if(!sv_name || !task)
        return;

    char http_body[512] = {0};
    char http_response[4096] = {0};
    char date[64] = {0};

    get_date(date);
    // Pagina de error 400
    read_file("root/templates/400.html", http_body, sizeof(http_body));
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

/********
 * FUNCIÓN: void http_403(char *sv_name, struct TODO *task)
 * ARGS_IN: char *sv_name - nombre del servidor,
 *          struct TODO *task - estructura de tarea con información sobre la petición.
 * DESCRIPCIÓN: Envía una respuesta HTTP 403 Forbidden al cliente.
 * ARGS_OUT: Ninguno (void).
 ********/
void http_403(char *sv_name, struct TODO *task)
{
    if(!sv_name || !task)
        return;

    char http_body[512] = {0};
    char http_response[4096] = {0};
    char date[64] = {0};

    get_date(date);
    // Página de error 403
    read_file("root/templates/403.html", http_body, sizeof(http_body));
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

/********
 * FUNCIÓN: void http_404(char *sv_name, struct TODO *task)
 * ARGS_IN: char *sv_name - nombre del servidor,
 *          struct TODO *task - estructura de tarea con información sobre la petición.
 * DESCRIPCIÓN: Envía una respuesta HTTP 404 Not Found al cliente.
 * ARGS_OUT: Ninguno (void).
 ********/
void http_404(char *sv_name, struct TODO *task)
{
    if(!sv_name || !task)
        return;

    char http_body[512] = {0};
    char http_response[4096] = {0};
    char date[64] = {0};

    get_date(date);
    // Página de error 404
    read_file("root/templates/404.html", http_body, sizeof(http_body));
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

/********
 * FUNCIÓN: void http_500(char *sv_name, int client_fd)
 * ARGS_IN: char *sv_name - nombre del servidor,
 *         int client_fd - descriptor de archivo del socket del cliente.
 * DESCRIPCIÓN: Envía una respuesta HTTP 500 Internal Server Error al cliente.
 * ARGS_OUT: Ninguno (void).
 * *******/
void http_500(char *sv_name, int client_fd)
{
    if(!sv_name || client_fd <= 0)
        return;

    char http_body[512] = {0};
    char http_response[4096] = {0};
    char date[64] = {0};

    get_date(date);
    // Página de error 500
    read_file("root/templates/500.html", http_body, sizeof(http_body));
    snprintf(http_response, sizeof(http_response),
             "HTTP/1.1 500 Internal Server Error\r\n"
             "Server: %s\r\n"
             "Date: %s\r\n"
             "Content-Type: text/html; charset=UTF-8\r\n"
             "Content-Length: %lu\r\n"
             "Connection: close\r\n"
             "\r\n"
             "%s",
             sv_name, date, strlen(http_body), http_body);
    // Enviar respuesta
    send(client_fd, http_response, strlen(http_response), 0);
}

/********
 * FUNCIÓN: void http_501(char *sv_name, struct TODO *task)
 * ARGS_IN: char *sv_name - nombre del servidor,
 *          struct TODO *task - estructura de tarea con información sobre la petición.
 * DESCRIPCIÓN: Envía una respuesta HTTP 501 Not Implemented al cliente.
 * ARGS_OUT: Ninguno (void).
 ********/
void http_501(char *sv_name, struct TODO *task)
{
    if(!sv_name || !task)
        return;

    char http_body[512] = {0};
    char http_response[4096] = {0};
    char date[64] = {0};

    get_date(date);
    // Página de error 501
    read_file("root/templates/501.html", http_body, sizeof(http_body));
    snprintf(http_response, sizeof(http_response),
             "%s 501 Not Implemented\r\n"
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

/********
 * FUNCIÓN: int parse_http_request(const char* buffer, size_t buflen, struct TODO* task)
 * ARGS_IN: const char* buffer - buffer que contiene la petición HTTP,
 *          size_t buflen - longitud del buffer,
 *          struct TODO* task - estructura de tarea donde almacenar la petición parseada.
 * DESCRIPCIÓN: Parsea la petición HTTP del buffer y almacena la información relevante en la estructura de tarea.
 * ARGS_OUT: int - devuelve 1 en caso de éxito, 0 si la petición es incompleta, o -1 si hay un error.
 ********/
int parse_http_request(const char *buffer, size_t buflen, struct TODO *task, FILE *logFile)
{
    if(!task || !logFile || !buffer || !buflen)
        return 0;
        
    const char *method, *path;
    int minor_version;
    struct phr_header headers[100];

    size_t method_len, path_len, num_headers = sizeof(headers) / sizeof(headers[0]);

    // Parsea la solicitud HTTP
    int pret = phr_parse_request(buffer, buflen, &method, &method_len, &path, &path_len,
                                 &minor_version, headers, &num_headers, 0);

    // Si la solicitud fue parseada exitosamente
    if (pret > 0)
    {
        // Copia el método, URI y versión del protocolo a la estructura TO-DO
        strncpy(task->verb, method, method_len);
        task->verb[method_len] = '\0';
        strncpy(task->uri, path, path_len);
        task->uri[path_len] = '\0';
        sprintf(task->version, "HTTP/1.%d", minor_version);

        // Tamaño de la solicitud
        task->len = buflen;

        // Argumentos de la solicitud
        if (strncmp(method, "GET", method_len) == 0)
        {
            // Para GET, buscamos '?' en el path.
            const char *args_start = strchr(path, '?');
            // Si hay argumentos
            if (args_start != NULL)
            {
                args_start++;
                // Copiar los argumentos de la solicitud a la estructura TO-DO
                size_t args_len = path_len - (args_start - path);
                if (args_len < sizeof(task->data))
                { // SEGFAULT
                    strncpy(task->data, args_start, args_len);
                    task->data[args_len] = '\0';
                }
                // ELiminar ? y argumentos de la URI
                task->uri[path_len - args_len - 1] = '\0';
            }
            else task->data[0] = '\0';
            task->dupdata[0] = '\0';
        }
        else if (strncmp(method, "POST", method_len) == 0)
        {
            // Para POST, buscamos el cuerpo del mensaje
            const char *body_start = buffer + pret;
            size_t body_len = buflen - pret;
            // Copiar el cuerpo del mensaje a la estructura TO-DO
            if (body_len < sizeof(task->data))
            {
                strncpy(task->data, body_start, body_len);
                task->data[body_len] = '\0';
            }
            else task->data[0] = '\0';
            // GET/POST
            const char *args_start = strchr(path, '?');
            // Si hay argumentos en la URI
            if (args_start != NULL && task->data[0] != '\0')
            {
                args_start++;
                // Copiar los argumentos de la solicitud a la estructura TO-DO
                size_t args_len = path_len - (args_start - path);
                if (args_len < sizeof(task->dupdata))
                { // SEGFAULT
                    strncpy(task->dupdata, args_start, args_len);
                    task->dupdata[args_len] = '\0';
                }
                // ELiminar ? y argumentos de la URI
                task->uri[path_len - args_len - 1] = '\0';
            }
            else task->dupdata[0] = '\0';
        }

        // Mostrar petición en consola del servidor
        writeToLog(logFile, "REQUEST", buffer, NULL);

        return 1; // Éxito
    }

    return pret; // Devuelve el resultado del parseo (0 para incompleto, -1 para error)
}

/********
* FUNCIÓN: void send_http_response(struct TODO *task, struct ServerConfig config)
* ARGS_IN: struct TODO *task - estructura de tarea con información sobre la petición,
*          struct ServerConfig config - configuración del servidor.
* DESCRIPCIÓN: Envía una respuesta HTTP al cliente basada en el verbo HTTP y la URI solicitada.
*              Comprueba si la URI intenta acceder a directorios fuera del root y, en caso afirmativo, envía un 403.
*              Si la URI es root, devuelve index.html. Concatena el root y la URI para obtener la ruta completa.
*              Luego, en función del verbo HTTP, llama a la función correspondiente para manejar la petición.
* ARGS_OUT: Ninguno (void).
********/
void send_http_response(struct TODO *task, struct ServerConfig config)
{
    if(!task || !config.sv_name || !config.root)
        return;
    
    int verb;

    // Comprobar que la petición no intente acceder a directorios fuera de root
    if (strstr(task->uri, "..") != NULL)
    {
        http_403(config.sv_name, task);
        return;
    }
    // Si se busca el root devolver index.html
    if (strcmp(task->uri, "/") == 0)
    {
        strcpy(task->uri, "/templates/index.html");
    }
    // Concatenar root y task->uri
    size_t needed_size = strlen(config.root) + strlen(task->uri) + 1; // +1 para el carácter nulo
    char *new_uri = malloc(needed_size);
    if (new_uri != NULL)
    {
        snprintf(new_uri, needed_size, "%s%s", config.root, task->uri);
        strncpy(task->uri, new_uri, needed_size);
        free(new_uri);
    }
    else
    {
        http_500(config.sv_name, task->client_sock);
        return;
    }
    verb = get_verb(task->verb);
    int ret = 0;
    switch (verb)
    {
    case 0: // GET
        if ((ret = method_get(config, task)) != 0)
        {
            if (ret == -1)
                http_404(config.sv_name, task);
            else if (ret == 1)
                http_400(config.sv_name, task);
            else
                http_500(config.sv_name, task->client_sock);
        }
        break;
    case 1: // POST
        if (task->data[0] == '\0' || method_post(config, task) != OK)
        {
            http_400(config.sv_name, task);
        }
        break;
    case 2: // OPTIONS
        if (method_options(config.sv_name, task) != OK)
        {
            http_400(config.sv_name, task);
        }
        break;
    default: // Otro verbo
        http_501(config.sv_name, task);
        break;
    }
}