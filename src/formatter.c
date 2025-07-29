/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda and contributors
 */
#include <stdio.h>
#include "formatter.h"

#include <string.h>

void escapeJSONString(const char *input, char *output, size_t outSize)
{
    size_t outIndex = 0;

    for (size_t i = 0; input[i] != '\0' && outIndex < outSize - 1; i++)
    {
        if (outIndex >= outSize - 2)
            break; // Leave space for null-terminator

        switch (input[i])
        {
        case '\"':
            if (outIndex < outSize - 2)
            {
                output[outIndex++] = '\\';
                output[outIndex++] = '\"';
            }
            break;
        case '\\':
            if (outIndex < outSize - 2)
            {
                output[outIndex++] = '\\';
                output[outIndex++] = '\\';
            }
            break;
        case '\n':
            if (outIndex < outSize - 2)
            {
                output[outIndex++] = '\\';
                output[outIndex++] = 'n';
            }
            break;
        case '\t':
            if (outIndex < outSize - 2)
            {
                output[outIndex++] = '\\';
                output[outIndex++] = 't';
            }
            break;
        default:
            output[outIndex++] = input[i];
            break;
        }
    }

    output[outIndex] = '\0';
}

void printLogText(LogEntry *entry, FILE *out)
{
    fprintf(out, "[%s] %s %s %s -> %d\n",
           entry->timestamp, entry->ip, entry->method, entry->url, entry->status);
}

void printLogJSON(LogEntry *entry, FILE *out)
{
    char escapedTimestamp[128] = {0};
    char escapedIP[128] = {0};
    char escapedMethod[32] = {0};
    char escapedURL[2048] = {0};
    char escapedUserAgent[2048] = {0};

    escapeJSONString(entry->timestamp, escapedTimestamp, sizeof(escapedTimestamp));
    escapeJSONString(entry->ip, escapedIP, sizeof(escapedIP));
    escapeJSONString(entry->method, escapedMethod, sizeof(escapedMethod));
    escapeJSONString(entry->url, escapedURL, sizeof(escapedURL));
    escapeJSONString(entry->userAgent, escapedUserAgent, sizeof(escapedUserAgent));

    fprintf(out, "  {\"timestamp\": \"%s\", \"ip\": \"%s\", \"method\": \"%s\", \"url\": \"%s\", \"status\": %d, \"userAgent\": \"%s\"}",
           escapedTimestamp, escapedIP, escapedMethod, escapedURL, entry->status, escapedUserAgent);
}

void printLogCSV(LogEntry *entry, FILE *out)
{
    fprintf(out, "\"%s\",\"%s\",\"%s\",\"%s\",%d,\"%s\"\n",
           entry->timestamp, entry->ip, entry->method, entry->url, entry->status, entry->userAgent);
}

