#include "../includes/log.h"

int isLogFileOpen = 0;

/********
 * FUNCIÓN: FILE* startLog(const char *logPath) 
 * DESCRIPCIÓN: Abre un fichero de log en modo escritura, si no existe lo crea.
 * ARGS_OUT: FILE* - Puntero al fichero de log
 ********/
FILE* startLog(const char *logPath) {
    FILE* logFile = fopen(logPath, "w");
    if (logFile == NULL) {
        perror("Error opening log file");
        return NULL;
    }
    isLogFileOpen = 1;
	return logFile;
}

/********
 * FUNCIÓN: void writeToLog(FILE* logFile, const char *level, const char *message)
 * ARGS_IN: char FILE* logFile - Puntero al fichero de log, const char *level - Nivel de log (INFO, WARNING, ERROR), const char *message - Mensaje a escribir en el log
 * DESCRIPCIÓN: Escribe un mensaje en el fichero de log, con el nivel de log especificado, si pool es NULL no es un hilo, no usamos semáforos
 * ARGS_OUT: -
 ********/
void writeToLog(FILE* logFile, const char *level, const char *message, struct Pool *pool) {
    time_t now;
    time(&now);
    char *timeStr = ctime(&now);
    timeStr[strlen(timeStr) - 1] = '\0';
    if (isLogFileOpen == 1 && logFile != NULL) {
        if (pool) pthread_mutex_lock(&(pool->logMutex));
        if (strcmp(level, "REQUEST") == 0)
            fprintf(logFile, "[%s] \n%s\n", timeStr, message);
        else
            fprintf(logFile, "[%s] %s: %s\n", timeStr, level, message);
        fflush(logFile);
        if (pool) pthread_mutex_unlock(&(pool->logMutex));
    }
}

/********
 * FUNCIÓN: void stopLog(FILE* logFile)
 * ARGS_IN: char FILE* logFile - Puntero al fichero de log
 * DESCRIPCIÓN: cierra el fichero de log
 * ARGS_OUT: -
 ********/
void stopLog(FILE* logFile) {
    if (isLogFileOpen == 1 && logFile != NULL) {
        fclose(logFile);
        logFile = NULL;
        isLogFileOpen = 0;
    }
}