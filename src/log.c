#include "../includes/log.h"

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
	return logFile;
}

/********
 * FUNCIÓN: void writeToLog(FILE* logFile, const char *level, const char *message)
 * ARGS_IN: char FILE* logFile - Puntero al fichero de log, const char *level - Nivel de log (INFO, WARNING, ERROR), const char *message - Mensaje a escribir en el log
 * DESCRIPCIÓN: Escribe un mensaje en el fichero de log, con el nivel de log especificado.
 * ARGS_OUT: -
 ********/
void writeToLog(FILE* logFile, const char *level, const char *message) {
    time_t now;
    time(&now);
    char *timeStr = ctime(&now);
    timeStr[strlen(timeStr) - 1] = '\0';
    if (strcmp(level, "REQUEST") == 0)
        fprintf(logFile, "[%s] \n%s\n", timeStr, message);
    else
        fprintf(logFile, "[%s] %s: %s\n", timeStr, level, message);
    fflush(logFile);
}

/********
 * FUNCIÓN: void stopLog(FILE* logFile)
 * ARGS_IN: char FILE* logFile - Puntero al fichero de log
 * DESCRIPCIÓN: cierra el fichero de log
 * ARGS_OUT: -
 ********/
void stopLog(FILE* logFile) {
    fclose(logFile);
}