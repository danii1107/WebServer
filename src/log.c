#include "../includes/log.h"

FILE* startLog(const char *logPath) {
    FILE* logFile = fopen(logPath, "w");
    if (logFile == NULL) {
        perror("Error opening log file");
        return NULL;
    }
	return logFile;
}

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

void stopLog(FILE* logFile) {
    fclose(logFile);
}