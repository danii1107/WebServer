#include "../includes/utils.h"

int getVerb(const char* verb) {
	if (strcmp(verb, "GET") == 0) return 0;
	if (strcmp(verb, "POST") == 0) return 1;
	if (strcmp(verb, "OPTIONS") == 0) return 2;
	return -1;
}