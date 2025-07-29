/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda and contributors
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logfire.h"
#include "parser.h"

/**
 * @brief Parses a log file in the common log format and processes each entry.
 *
 * This function opens the specified log file, reads it line by line, and attempts to parse
 * each line according to the common log format, extracting fields such as IP address,
 * timestamp, request, status code, size, referrer, and user agent. For each successfully
 * parsed line, it constructs a formatted message and calls the addLog function to process
 * the log entry.
 *
 * @param filename The path to the log file to be parsed.
 *
 * @note Lines that do not match the expected format are skipped.
 * @note The function prints an error message to stderr if the file cannot be opened.
 */
void parseLogFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Could not open file: %s\n", filename);
        return;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file))
    {
        char ip[64], timestamp[128], request[512], status[8], size[16], referrer[256], user_agent[256];

        int matched = sscanf(line,
                             "%63s - - [%127[^]]] \"%511[^\"]\" %7s %15s \"%255[^\"]\" \"%255[^\"]\"",
                             ip, timestamp, request, status, size, referrer, user_agent); // Parse common log format with request and user agent
        
        if (matched == 7)
        {
            char message[1024];
            snprintf(message, sizeof(message), "%s [%s] \"%s\" %s %s \"%s\"",
                     ip, timestamp, request, status, size, user_agent);
            addLog(timestamp, ip, request, message, atoi(status), user_agent);
        }
        else
        {
            continue; // Skip lines that do not match the expected format
        }
    }

    fclose(file);
}
