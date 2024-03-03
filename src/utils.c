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
    }

    if (pid == 0) { 
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO); 
        close(pipefd[1]); 

        execl(script_path, script_path, data, NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else { 
        close(pipefd[1]); 

        wait(NULL); 

        fp = fdopen(pipefd[0], "r");
        if (!fp) {
            perror("fdopen");
            exit(EXIT_FAILURE);
        }

        fread(response, 1, response_size, fp);
        fclose(fp);
    }
}