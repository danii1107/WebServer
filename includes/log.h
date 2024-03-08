#ifndef LOG_H
#define LOG_H

#include "types.h"

FILE* startLog(const char *logPath);
void writeToLog(FILE* logFile, const char *level, const char *message);
void stopLog(FILE* logFile);

#endif