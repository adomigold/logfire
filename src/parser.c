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
 * @brief Parses a single log line in Apache or Nginx combined log format.
 *
 * This function attempts to extract key fields from a log line, including IP address,
 * timestamp, HTTP method, URL, protocol, status code, and User-Agent. The referrer
 * field is ignored. The extracted values are stored in the provided LogEntry struct.
 *
 * @param line      The input log line as a null-terminated string.
 * @param out       Pointer to a LogEntry struct to be filled with parsed data.
 * @param errmsg    Buffer to receive an error message if parsing fails (can be NULL).
 * @param errmsg_sz Size of the errmsg buffer.
 * @return          1 if parsing was successful and required fields were found, 0 otherwise.
 */
int parse_apache_or_nginx(const char *line, LogEntry *out, char *errmsg, size_t errmsg_sz)
{
    memset(out, 0, sizeof(*out)); // Reset out

    char ip[64] = {0}, timebuf[64] = {0}, method[16] = {0}, url[1024] = {0}, proto[32] = {0}, ua[1024] = {0}; // Example line: 83.149.9.216 - - [17/May/2015:10:05:03 +0000] "GET /path HTTP/1.1" 200 123 "-" "UA..."
    int status = 0;

    // NOTE: Some servers put the referrer in quotes before the User-Agent; the pattern above skips it with %*[^\" ].
    // This parser will continue to ignore the referrer and only extract the User-Agent.
    int matched = sscanf(line,
                         "%63s - - [%63[^]]] \"%15s %1023s %31[^\"]\" %d %*s %*[^\" ] \"%1023[^\"]\"",
                         ip, timebuf, method, url, proto, &status, ua); // we use scansets to grab inside [] and ""

    if (matched < 6)

    { // be lenient; require at least ip,time,method,url,proto,status
        if (errmsg && errmsg_sz)
            snprintf(errmsg, errmsg_sz, "sscanf matched %d fields", matched);
        return 0;
    }

    strncpy(out->ip, ip, sizeof(out->ip) - 1);
    strncpy(out->timestamp, timebuf, sizeof(out->timestamp) - 1);
    strncpy(out->method, method, sizeof(out->method) - 1);
    strncpy(out->url, url, sizeof(out->url) - 1);
    out->status = status;
    strncpy(out->userAgent, ua, sizeof(out->userAgent) - 1);

    return 1;
}