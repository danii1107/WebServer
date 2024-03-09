/**
 * @brief Implementa la funcionalidad de funciones auxiliares
 * @file utils.c
 * @author Daniel Birsan y Diego Rodrigo
 * @version 1.0
 * @date 13/03/2024
 */

#include "../includes/utils.h"

/********
 * FUNCIÓN: int get_verb(const char* verb)
 * ARGS_IN: const char* verb - Verbo HTTP como cadena de caracteres ("GET", "POST", "OPTIONS").
 * DESCRIPCIÓN: Comprueba el verbo HTTP proporcionado y devuelve un entero asociado a ese verbo.
 * ARGS_OUT: int - Devuelve 0 para "GET", 1 para "POST", 2 para "OPTIONS", y -1 si el verbo no es reconocido.
 ********/
int get_verb(const char *verb)
{
    if (!verb)
        return -1;
    if (strcmp(verb, "GET") == 0)
        return 0;
    if (strcmp(verb, "POST") == 0)
        return 1;
    if (strcmp(verb, "OPTIONS") == 0)
        return 2;
    return -1;
}

/********
 * FUNCIÓN: void get_date(char *date)
 * ARGS_IN: char *date - Puntero a una cadena de caracteres donde almacenar la fecha actual.
 * DESCRIPCIÓN: Obtiene la fecha y hora actuales en formato GMT y las escribe en la cadena de caracteres apuntada por date.
 ********/
void get_date(char *date)
{
    time_t tm = time(0);
    struct tm *t = gmtime(&tm);
    strftime(date, 128, "%a, %d %b %Y %H:%M:%S %Z", t);
}

/********
 * FUNCIÓN: int get_content_type(char *uri, char *ct)
 * ARGS_IN: char *uri - URI del recurso solicitado, char *ct - Puntero a una cadena de caracteres donde almacenar el tipo de contenido.
 * DESCRIPCIÓN: Determina el tipo de contenido (MIME type) del recurso solicitado basándose en su extensión de archivo y lo almacena en ct.
 * ARGS_OUT: OK si todo ha ido bien, ERROR si se ha producido un error.
 ********/
STATUS get_content_type(char *uri, char *ct)
{
    if(!uri || !ct)
        return ERROR;

    char *ext = strrchr(uri, '.');
    if (!ext)
        return ERROR;

    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0)
    {
        strcpy(ct, "text/html");
    }
    else if (strcmp(ext, ".txt") == 0)
    {
        strcpy(ct, "text/plain");
    }
    else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
    {
        strcpy(ct, "image/jpeg");
    }
    else if (strcmp(ext, ".gif") == 0)
    {
        strcpy(ct, "image/gif");
    }
    else if (strcmp(ext, ".mpeg") == 0 || strcmp(ext, ".mpg") == 0)
    {
        strcpy(ct, "video/mpeg");
    }
    else if (strcmp(ext, ".doc") == 0 || strcmp(ext, ".docx") == 0)
    {
        strcpy(ct, "application/msword");
    }
    else if (strcmp(ext, ".pdf") == 0)
    {
        strcpy(ct, "application/pdf");
    }
    else
    {
        return ERROR;
    }

    return OK;
}

/********
 * FUNCIÓN: int read_file(const char *path, char *buffer, size_t buffer_size)
 * ARGS_IN: const char *path - Ruta al archivo a leer, char *buffer - Buffer donde almacenar el contenido del archivo, size_t buffer_size - Tamaño del buffer.
 * DESCRIPCIÓN: Lee el contenido de un archivo y lo almacena en un buffer. El número de bytes leídos no superará buffer_size.
 * ARGS_OUT: int - Devuelve el número de bytes leídos en caso de éxito, -1 si el archivo no puede ser abierto.
 ********/
int read_file(const char *path, char *buffer, size_t buffer_size)
{
    if(!path)
        return -1;

    FILE *file = fopen(path, "r");
    if (!file)
    {
        return -1;
    }

    size_t bytes_read = fread(buffer, 1, buffer_size, file);
    fclose(file);
    return bytes_read;
}

/********
 * FUNCIÓN: void replace_char(char *str, char find, char replace)
 * ARGS_IN: char *str - Cadena de caracteres donde reemplazar el carácter, char find - Carácter a reemplazar, char replace - Carácter por el que reemplazar.
 * DESCRIPCIÓN: Reemplaza todas las ocurrencias de un carácter en una cadena por otro carácter.
 * ARGS_OUT: STATUS - OK si todo ha ido bien, ERROR si se ha producido un error.
 ********/
STATUS replace_char(char *str, char find, char replace) {
    if(!str || !find || !replace)
        return ERROR;
    char *current_pos = strchr(str, find);
    while (current_pos) {
        *current_pos = replace;
        current_pos = strchr(current_pos + 1, find);
    }
    return OK;
}

/********
 * FUNCIÓN: void parse_args(const char *args, char *parsed_args[], size_t parsed_args_size)
 * ARGS_IN: const char *args - Cadena de caracteres con los argumentos a parsear, char *parsed_args[] - Array de cadenas donde almacenar los argumentos parseados, size_t parsed_args_size - Tamaño del array parsed_args.
 * DESCRIPCIÓN: Tokeniza una cadena de argumentos delimitados por '&' y almacena cada argumento en un array de cadenas.
 * ARGS_OUT: STATUS - OK si todo ha ido bien, ERROR si se ha producido un error.
 ********/
STATUS parse_args(const char *args, char *parsed_args[], size_t parsed_args_size) {
    char *args_copy = strdup(args); // Hacemos una copia de args porque strtok modifica la cadena original
    char *token = strtok(args_copy, "&");
    size_t i = 0;
    while (token && i < parsed_args_size) { // Ajuste para dejar espacio para el NULL final
        char *value = strchr(token, '=');
        if (value) {
            value++; // Nos movemos más allá del '='
            if (replace_char(value, '+', ' ') != OK) // Reemplazamos '+' por espacios
            {
                free(args_copy);
                return ERROR;
            }
            parsed_args[i] = strdup(value);
            i++;
        }
        token = strtok(NULL, "&");
    }
    parsed_args[i] = NULL; // Asegurarse de que el array termine en NULL
    
    free(args_copy); // Liberamos la copia de args
    return OK;
}

/********
 * FUNCIÓN: int execute_script(char *script_path, char *data[], char **response, ssize_t *response_size)
 * ARGS_IN: char *script_path - Ruta al script a ejecutar, char *data[] - Array de cadenas con los argumentos para el script, char **response - Puntero a una cadena donde almacenar la salida del script, ssize_t *response_size - Puntero a una variable donde almacenar el tamaño de la respuesta.
 * DESCRIPCIÓN: Ejecuta un script externo (Python o PHP), pasando los argumentos proporcionados y capturando su salida.
 * ARGS_OUT: OK si todo ha ido bien, ERROR si se ha producido un error.
 ********/
STATUS execute_script(char *script_path, char *data[], char **response, ssize_t *response_size)
{
    int pipefd[2];
    char executable[10];
    pid_t pid;

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return ERROR;
    }

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        return ERROR;
    }
    if (pid == 0)
    {
        close(pipefd[0]);               // Cerrar el extremo de lectura del pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirigir la salida estándar al pipe
        dup2(pipefd[1], STDERR_FILENO); // Redirigir la salida de error al pipe
        close(pipefd[1]);               // Cerrar el extremo de escritura del pipe

        // Ejecutar el script python o php sin necesidad de exportar el entorno a ejecutar
        if (strstr(script_path, ".py"))
        {
            strcpy(executable, "python3");
        }
        else
        {
            strcpy(executable, "php");
        }
        
        size_t data_count = 0;
        if (data)
            for (data_count = 0; data[data_count]; data_count++);

        // Reservar espacio para el nombre del ejecutable, script_path, argumentos, y NULL
        char *argv[data_count + 3];
        argv[0] = executable;
        argv[1] = script_path;

        // Llenar argv con los argumentos
        for (size_t i = 0; i < data_count; i++)
            argv[i + 2] = data[i];
        argv[data_count + 2] = NULL; // Terminar argv con NULL

        execvp(executable, argv);
        // Si execvp retorna, ocurrió un error
        exit(EXIT_FAILURE);
    }
    else
    {
        close(pipefd[1]); // Cerrar el extremo de escritura del pipe
        char buffer[16];
        ssize_t bytesRead;
        *response_size = 0;

        // Leer la salida del script desde el pipe
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[bytesRead] = '\0';
            if (strstr(buffer, "\r\n"))
            {
                bytesRead -= 2; // Excluir "\r\n" del tamaño de la respuesta
            }

            // Añadir la salida del script al buffer de respuesta
            *response = realloc(*response, *response_size + bytesRead + 1);
            memcpy(*response + *response_size, buffer, bytesRead);
            (*response)[*response_size + bytesRead] = '\0';
            *response_size += bytesRead;

            if (strstr(buffer, "\r\n"))
                break;
        }

        close(pipefd[0]);
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }
        else
            return ERROR;
    }
    return OK;
}