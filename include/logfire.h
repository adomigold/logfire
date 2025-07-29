/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda and contributors
 */
#ifndef LOGFIRE_H
#define LOGFIRE_H
#include "parser.h"
#include "formatter.h"
#include "logstore.h"

#define MAX_LOGS 10000
#define MAX_LINE_LENGTH 1024

extern LogEntry logEntries[MAX_LOGS];
extern int logCount;

extern enum OutputFormat currentFormat;
enum OutputFormat parseFormat(const char *format);

void addLog(const char *timestamp, const char *ip, const char *method,
            const char *url, int status, const char *userAgent);
void searchLogs(const char *keyword, enum OutputFormat currentFormat, FILE *out);

#endif // LOGFIRE_H
