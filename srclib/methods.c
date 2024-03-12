/**
 * @brief Implementa la funcionalidad de los metodos (get,post,options)
 * @file methods.c
 * @author Daniel Birsan y Diego Rodrigo
 * @version 1.0
 * @date 13/03/2024
 */

#include "../includes/methods.h"

// Funciones auxiliares privadas
int scripts_aux(int method, char *http_response, char *date, char *sv_name, struct TODO *task);

/********
* FUNCIÓN: int method_get(struct ServerConfig config, struct TODO *task)
* ARGS_IN: struct ServerConfig config - Configuración del servidor,
*          struct TODO *task - Tarea con información sobre la solicitud HTTP.
* DESCRIPCIÓN: Maneja solicitudes GET, sirviendo archivos estáticos o ejecutando scripts (PHP, Python).
*              Si el URI incluye un script, lo ejecuta y devuelve su salida. Si es una solicitud de archivo,
*              intenta abrir y enviar el archivo solicitado. Si los datos son innecesarios o el archivo no existe,
*              puede retornar un error.
* ARGS_OUT: OK si todo ha ido bien, ERROR si se ha producido un error.
********/
STATUS  method_get(struct ServerConfig config, struct TODO *task) {

    if(!config.sv_name || !task){
        return ERROR;
    }
    
	char http_response[16384];

    char date[64];
    int script = 0;
    
    // Comprobar si es un script
    if (strstr(task->uri, ".py") || strstr(task->uri, ".php")) script = 1;
    
    get_date(date);
    int ret = 0;
    // Comprobar si es un script y ejecutarlo
    if (script == 1) {
        if ((ret = scripts_aux(0, http_response, date, config.sv_name, task)) != 0)
            return ret;
    } else { // Si no hay datos o son innecesarios, buscar el archivo solicitado
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
        if (fseek(file, 0, SEEK_END) != 0) {
            fclose(file);
            return -1;
        }
        long file_size = ftell(file);
        if (file_size == -1) {
            fclose(file);
            return -1;
        }
        rewind(file);
        
        // Leer el contenido del archivo
        char *file_content = malloc(file_size);
        if (!file_content) {
            fclose(file);
            return 2;
        }
        if((int) fread(file_content, file_size, 1, file) < 0){
            free(file_content);
            fclose(file);
            return 2;
        }
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
* ARGS_OUT: OK si todo ha ido bien, ERROR si se ha producido un error.
********/
STATUS method_post(struct ServerConfig config, struct TODO *task)
{
    if(!config.sv_name || !task){
        return ERROR;
    }
    
    char http_response[5000];
    char date[64];

    get_date(date);
    // Comprobar si es un script y ejecutarlo
    if (strstr(task->uri, ".py") || strstr(task->uri, ".php")) {
        if (scripts_aux(1, http_response, date, config.sv_name, task) != OK)
            return ERROR;
    } else { // Suponmos poder enviar un POST sin necesidad de ser un script, en servidores reales pueden ejecutarse estos post, 
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
    }
    // Enviar respuesta
    send(task->client_sock, http_response, strlen(http_response), 0);
    return OK;
}

/********
* FUNCIÓN: void method_options(char *sv_name, struct TODO *task)
* ARGS_IN: char *sv_name - Nombre del servidor,
*          struct TODO *task - Tarea con información sobre la solicitud HTTP.
* DESCRIPCIÓN: Maneja solicitudes OPTIONS, enviando una respuesta que lista los métodos HTTP permitidos (GET, POST, OPTIONS).
* ARGS_OUT: OK si todo ha ido bien, ERROR si se ha producido un error.
********/
STATUS method_options(char *sv_name, struct TODO *task) {

    if(!sv_name || !task){
        return ERROR;
    }
    
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
    return OK;
}

/********
* FUNCIÓN: int scripts_aux(char *http_response, char *date, char *sv_name, struct TODO *task)
* ARGS_IN:
*          int method - almacena get o post en forma de int 
*          char *http_response - String que almacena la respuesta del servidor,
*          char *date - String que almacena la fecha y hora actuales en formato GMT,
*          char *sv_name - Nombre del servidor
*          struct TODO *task - Tarea con información sobre la solicitud HTTP.
* DESCRIPCIÓN: Llama a la función execute_script para ejecutar el script solicitado y almacenar su salida en http_response.
* ARGS_OUT: 0 si todo ha ido bien, -1 si 404 o 1 si 400, 2 si 500.
********/
int scripts_aux(int method, char *http_response, char *date, char *sv_name, struct TODO *task)
{
    char *script_output = NULL;
    char **dup_parsed_args = NULL;
    ssize_t script_output_size = 0;
    size_t count = 0;

    // Reservar memoria para array de argumentos y tokenizarlos
    if (task->data[0] != '\0' || method == 1) // Script con args
    {
        for (int i = 0; task->data[i]; i++) {
            if (task->data[i] == '&') count++;
        }
        char *parsed_args[count + 2];
        if (parse_args(task->data, parsed_args, count + 1) != OK)
            return 2;
        if (task->dupdata[0] != '\0') {
            for (int i = 0, count = 0; task->dupdata[i]; i++) {
                if (task->dupdata[i] == '&') count++;
            }
            dup_parsed_args = calloc(count + 2, sizeof(char*));
            if (dup_parsed_args == NULL) {
                for (size_t i = 0; i < count; i++) {
                    free(parsed_args[i]);
                }
                return 2;
            }
            if (parse_args(task->dupdata, dup_parsed_args, count + 1) != OK)
            {
                int i = 0;
                while (parsed_args[i] != NULL) {
                    free(parsed_args[i]);
                    i++;
                }
                i = 0;
                while (dup_parsed_args[i] != NULL) {
                    free(dup_parsed_args[i]);
                    i++;
                }
                if (dup_parsed_args) free(dup_parsed_args);
                return 2;
            }
        }
        if (execute_script(method, task->uri, parsed_args, &script_output, &script_output_size, dup_parsed_args) != OK)
        {
            if (script_output) free(script_output);
            int i = 0;
            while (parsed_args[i] != NULL) {
                free(parsed_args[i]);
                i++;
            }
            i = 0;
            while (dup_parsed_args[i] != NULL) {
                free(dup_parsed_args[i]);
                i++;
            }
            if (dup_parsed_args) free(dup_parsed_args);
            return -1;
        }
        int i = 0;
        while (parsed_args[i] != NULL) {
            free(parsed_args[i]);
            i++;
        }
        i = 0;
        if (dup_parsed_args)
        {
            while (dup_parsed_args[i] != NULL) {
                free(dup_parsed_args[i]);
                i++;
            }
            free(dup_parsed_args);
        }
    } else { // Script sin args
        if (execute_script(method, task->uri, NULL, &script_output, &script_output_size, NULL) != OK)
        {
            if (script_output) free(script_output);
            return -1;
        }
    }
    // Armar respuesta script
    sprintf(http_response, "%s 200 OK\r\n"
                    "Date: %s\r\n"
                    "Server: %s\r\n"
                    "Content-Type: text/plain; charset=UTF-8\r\n"
                    "Content-Length: %lu\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "%s\n",
                    task->version, date, sv_name, script_output_size, script_output);
    
    if (script_output) free(script_output);

    return 0;
}