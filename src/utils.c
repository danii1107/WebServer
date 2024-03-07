#include "../includes/utils.h"

// Comprobar todos los campos de la edd

int get_verb(const char* verb) {
	if (strcmp(verb, "GET") == 0) return 0;
	if (strcmp(verb, "POST") == 0) return 1;
	if (strcmp(verb, "OPTIONS") == 0) return 2;
	return -1;
}

void get_date(char *date) {
    time_t tm = time(0);	
    struct tm *t = gmtime(&tm);
    strftime(date, 128, "%a, %d %b %Y %H:%M:%S %Z", t);
}

// Obtiene el tipo de contenido del recurso solicitado
int get_content_type(char *uri, char *ct) {
    char *ext = strrchr(uri, '.');
    if (!ext) {
        return -1;
    }

    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) {
        strcpy(ct, "text/html");
    } else if (strcmp(ext, ".txt") == 0) {
        strcpy(ct, "text/plain");
    } else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
        strcpy(ct, "image/jpeg");
    } else if (strcmp(ext, ".gif") == 0) {
        strcpy(ct, "image/gif");
    } else if (strcmp(ext, ".mpeg") == 0 || strcmp(ext, ".mpg") == 0) {
        strcpy(ct, "video/mpeg");
    } else if (strcmp(ext, ".doc") == 0 || strcmp(ext, ".docx") == 0) {
        strcpy(ct, "application/msword");
    } else if (strcmp(ext, ".pdf") == 0) {
        strcpy(ct, "application/pdf");
    } else {
        return -1;
    }

    return 0;
}

// Lee el fichero en el buffer recibido
int read_file(const char *path, char *buffer, size_t buffer_size) {
    FILE *file = fopen(path, "r");
    if (!file) {
        return -1;
    }

    size_t bytes_read = fread(buffer, 1, buffer_size, file);
    fclose(file);
    return bytes_read;
}

// Tokenizar la cadena de argumentos  get y post y guardarlos en un array
void parse_args(const char *args, char *parsed_args[], size_t parsed_args_size) {
    char *args_copy = strdup(args); // Hacemos una copia de args porque strtok modifica la cadena original
    char *token = strtok(args_copy, "&");
    size_t i = 0;
    while (token && i < parsed_args_size) { // Ajuste para dejar espacio para el NULL final
    char *value = strchr(token, '=');
        if (value) {
            value++;
            parsed_args[i] = strdup(value);
            i++;
        }
        token = strtok(NULL, "&");
    }
    parsed_args[i] = NULL; // Asegurarse de que el array termine en NULL

    free(args_copy); // Liberamos la copia de args
}

// EJecutar script
int execute_script(char *script_path, char *data[], char **response, ssize_t *response_size) {
    int pipefd[2];
    char executable[10];
    pid_t pid;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } if (pid == 0) { 
        close(pipefd[0]); // Cerrar el extremo de lectura del pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirigir la salida estándar al pipe
        dup2(pipefd[1], STDERR_FILENO); // Redirigir la salida de error al pipe
        close(pipefd[1]); // Cerrar el extremo de escritura del pipe

        // Ejecutar el script python o php sin necesidad de exportar el entorno a ejecutar
        if (strstr(script_path, ".py")) {
            strcpy(executable, "python3");
        } else {
            strcpy(executable, "php");
        }

        size_t data_count;
        for (data_count = 0; data[data_count]; data_count++);

        // Reservar espacio para el nombre del ejecutable, script_path, argumentos, y NULL
        char *argv[data_count + 3];
        argv[0] = executable;
        argv[1] = script_path;

        // Llenar argv con los argumentos
        for (size_t i = 0; i < data_count; i++) {
            argv[i + 2] = data[i];
        }
        argv[data_count + 2] = NULL; // Terminar argv con NULL

        execvp(executable, argv);
        // Si execvp retorna, ocurrió un error
        exit(EXIT_FAILURE);

    } else {
        close(pipefd[1]); // Cerrar el extremo de escritura del pipe
        char buffer[16];
        ssize_t bytesRead;
        *response_size = 0;
        
        // Leer la salida del script desde el pipe
        while ((bytesRead = read(pipefd[0], buffer, sizeof(buffer)-1)) > 0) {
            buffer[bytesRead] = '\0'; 
            if (strstr(buffer, "\r\n")) {
                bytesRead -= 2; // Excluir "\r\n" del tamaño de la respuesta
            }

            // Añadir la salida del script al buffer de respuesta
            *response = realloc(*response, *response_size + bytesRead + 1);
            memcpy(*response + *response_size, buffer, bytesRead);
            (*response)[*response_size + bytesRead] = '\0';
            *response_size += bytesRead;

            if (strstr(buffer, "\r\n")) break;
        }
        
        close(pipefd[0]);
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -1; 
        }
    }
}