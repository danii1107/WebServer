#ifndef LOG_H
#define LOG_H

#include "types.h"
#include "pool.h"

/********
 * FUNCIÓN: FILE* startLog(const char *logPath) 
 * DESCRIPCIÓN: Abre un fichero de log en modo escritura, si no existe lo crea.
 * ARGS_OUT: FILE* - Puntero al fichero de log
 ********/
FILE* startLog(const char *logPath);

/********
 * FUNCIÓN: void writeToLog(FILE* logFile, const char *level, const char *message)
 * ARGS_IN: char FILE* logFile - Puntero al fichero de log, const char *level - Nivel de log (INFO, WARNING, ERROR), const char *message - Mensaje a escribir en el log
 * DESCRIPCIÓN: Escribe un mensaje en el fichero de log, con el nivel de log especificado, si pool es NULL no es un hilo, no usamos semáforos
 * ARGS_OUT: -
 ********/
void writeToLog(FILE* logFile, const char *level, const char *message, struct Pool *pool);

/********
 * FUNCIÓN: void stopLog(FILE* logFile)
 * ARGS_IN: char FILE* logFile - Puntero al fichero de log
 * DESCRIPCIÓN: cierra el fichero de log
 * ARGS_OUT: -
 ********/
void stopLog(FILE* logFile);

#endif