#ifndef QUERY_H
#define QUERY_H
#include "logstore.h"

int matches(const LogEntry *e, const char *needle, int ci); // returns 1 if the entry matches 'needle', 0 otherwise. Case-insensitive if ci!=0.

#endif
