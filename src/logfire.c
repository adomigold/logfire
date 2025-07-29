/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda and contributors
 */
#include <stdio.h>
#include <string.h>
#include "logfire.h"

LogEntry logEntries[MAX_LOGS];
int logCount = 0;

/**
 * @brief Adds a new log entry to the logEntries array.
 *
 * This function copies the provided log details (timestamp, IP address, HTTP method,
 * URL, status code, and user agent) into the next available slot in the logEntries array,
 * provided the maximum number of logs (MAX_LOGS) has not been reached. Each string field
 * is safely copied with null-termination to prevent buffer overflows.
 *
 * @param timestamp   The timestamp of the log entry.
 * @param ip          The IP address associated with the log entry.
 * @param method      The HTTP method used in the request.
 * @param url         The URL requested.
 * @param status      The HTTP status code returned.
 * @param userAgent   The user agent string from the request.
 *
 * @note If the logEntries array is full, an error message is printed to stderr and
 *       the log entry is not added.
 */
void addLog(const char *timestamp, const char *ip, const char *method,
            const char *url, int status, const char *userAgent)
{
    if (logCount < MAX_LOGS)
    {
        strncpy(logEntries[logCount].timestamp, timestamp, sizeof(logEntries[logCount].timestamp) - 1);
        logEntries[logCount].timestamp[sizeof(logEntries[logCount].timestamp) - 1] = '\0'; // Ensure null-termination

        strncpy(logEntries[logCount].ip, ip, sizeof(logEntries[logCount].ip) - 1);
        logEntries[logCount].ip[sizeof(logEntries[logCount].ip) - 1] = '\0'; // Ensure null-termination

        strncpy(logEntries[logCount].method, method, sizeof(logEntries[logCount].method) - 1);
        logEntries[logCount].method[sizeof(logEntries[logCount].method) - 1] = '\0'; // Ensure null-termination

        strncpy(logEntries[logCount].url, url, sizeof(logEntries[logCount].url) - 1);
        logEntries[logCount].url[sizeof(logEntries[logCount].url) - 1] = '\0'; // Ensure null-termination

        logEntries[logCount].status = status;

        strncpy(logEntries[logCount].userAgent, userAgent, sizeof(logEntries[logCount].userAgent) - 1);
        logEntries[logCount].userAgent[sizeof(logEntries[logCount].userAgent) - 1] = '\0'; // Ensure null-termination

        logCount++;
    }
    else
    {
        fprintf(stderr, "Log limit reached. Cannot add more logs.\n");
    }
}

/**
 * @brief Searches through the log entries for a given keyword and prints matching logs.
 *
 * This function iterates over all available log entries and checks if the specified keyword
 * is present in any of the following fields: method, URL, user agent, timestamp, or IP address.
 * If a match is found, the log entry is printed using the specified output format.
 * If no matches are found, a message is printed indicating that no logs matched the keyword.
 *
 * @param keyword The keyword to search for within the log entries.
 * @param currentFormat The output format to use when printing matching log entries.
 */
void searchLogs(const char *keyword, enum OutputFormat currentFormat, FILE *out)
{
    int found = 0;

    if (currentFormat == JSON)
        fprintf(out, "[\n");

    for (int i = 0; i < logCount; i++)
    {
        if (strstr(logEntries[i].method, keyword) != NULL ||
            strstr(logEntries[i].url, keyword) != NULL ||
            strstr(logEntries[i].userAgent, keyword) != NULL ||
            strstr(logEntries[i].timestamp, keyword) != NULL ||
            strstr(logEntries[i].ip, keyword) != NULL)
        {
            if (currentFormat == JSON && found > 0)
                fprintf(out, ",\n");

            printFormatted(&logEntries[i], currentFormat, out);
            found++;
        }
    }

    if (currentFormat == JSON)
        fprintf(out, "\n]\n");

    if (found == 0)
        fprintf(stderr, "No logs found matching \"%s\".\n", keyword);
}

