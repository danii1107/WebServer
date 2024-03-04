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

void execute_script(const char *script_path, const char *data, char *response, size_t response_size) {
    int pipefd[2];
    pid_t pid;
    FILE *fp;

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
        close(pipefd[1]); // Cerrar el extremo de escritura del pipe

        execl(script_path, script_path, data, NULL); // Ejecutar el script
        perror("execl");
        exit(EXIT_FAILURE);
    } else { 
        close(pipefd[1]); // Cerrar el extremo de escritura del pipe

        wait(NULL); // Esperar a que el hijo termine

        fp = fdopen(pipefd[0], "r");
        if (!fp) {
            perror("fdopen");
            exit(EXIT_FAILURE);
        }

        fread(response, 1, response_size, fp); // Leer la salida del script
        fclose(fp);
    }
}