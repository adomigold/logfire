/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda and contributors
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"
#include "parser.h"
#include "query.h"
#include "formatter.h"
#include "jsonout.h"

/**
 * read_line_dyn - Reads a line of arbitrary length from the given file pointer.
 *
 * This function dynamically allocates and resizes a buffer to read a line
 * from the specified file stream `fp`. It reads characters until a newline
 * character or EOF is encountered. The newline character, if present, is
 * replaced with a null terminator. The returned buffer must be freed by the caller.
 *
 * @param fp: Pointer to a FILE object to read from.
 *
 * @return Pointer to a dynamically allocated null-terminated string containing
 *         the line read (without the newline character), or NULL on EOF or error.
 */
char *read_line_dyn(FILE *fp)
{
    size_t cap = 4096, len = 0;

    char *buf = (char *)malloc(cap);

    if (!buf)
        return NULL;

    for (;;)
    {
        if (fgets(buf + len, (int)(cap - len), fp) == NULL)
        {
            if (len == 0)
            {
                free(buf);
                return NULL;
            }
            break;
        }

        len += strlen(buf + len);

        if (len && buf[len - 1] == '\n')
        {
            buf[len - 1] = '\0';
            break;
        }

        if (cap - len < 2)
        {
            cap *= 2;
            char *nbuf = (char *)realloc(buf, cap);
            if (!nbuf)
            {
                free(buf);
                return NULL;
            }
            buf = nbuf;
        }
    }
    return buf;
}

/**
 * @brief Processes a stream of log lines, parses them, and outputs in the specified format.
 *
 * Reads lines from the input stream, attempts to parse each as an Apache or Nginx log entry,
 * and writes the output in JSON, CSV, or plain text format to the output stream. Optionally
 * filters entries by a search term and supports case-insensitive matching. Handles parse
 * failures according to the strictness option and prints a summary to stderr.
 *
 * @param in        Input file stream to read log lines from.
 * @param label     Optional label for the input source, used in warnings and summary.
 * @param opt       Pointer to CLIOptions struct specifying output format, search term, and options.
 * @param out       Output file stream to write formatted log entries.
 */
void process_stream(FILE *in, const char *label, const CLIOptions *opt, FILE *out)
{
    long long total = 0, parsed = 0, failed = 0;
    int opened_json = 0, first_json = 1;

    /* ---- Parse field-based query once (if provided) ---- */
    Query q;
    int use_q = 0;
    char qerr[128] = {0};

    if (opt->query && *opt->query)
    {
        if (!query_parse(opt->query, opt->case_insensitive, &q, qerr, sizeof(qerr)))
        {
            fprintf(stderr, "query parse error: %s\n", qerr);
            // You can choose to return early, or just fall back to keyword search:
            // return;
        }
        else
        {
            use_q = 1;
        }
    }

    /* JSON array opening (batch mode) */
    if (opt->format == FORMAT_JSON)
    {
        fprintf(out, "[");
        opened_json = 1;
    }

    for (;;)
    {
        char *line = read_line_dyn(in);
        if (!line)
            break;
        total++;

        LogEntry e;
        char perr[256] = {0};

        if (parse_apache_or_nginx(line, &e, perr, sizeof(perr)))
        {
            int ok = 1;

            if (use_q)
            {
                ok = query_match(&e, &q);
            }
            else if (opt->query && *opt->query)
            {
                ok = matches(&e, opt->query, opt->case_insensitive);
            }
            else
            {
                ok = 1; // no filters -> print all
            }

            if (ok)
            {
                if (opt->format == FORMAT_JSON)
                {
                    if (!first_json)
                        fprintf(out, ",");
                    printLogJSON(&e, out); // your signature: (LogEntry*, FILE*)
                    first_json = 0;
                }
                else if (opt->format == FORMAT_CSV)
                {
                    printLogCSV(&e, out);
                    fputc('\n', out);
                }
                else
                {
                    printLogText(&e, out);
                    fputc('\n', out);
                }
            }
            parsed++;
        }
        else
        {
            failed++;
            if (opt->strict)
            {
                fprintf(stderr, "[warn] parse failed (%s): %s\n",
                        label ? label : "-", perr[0] ? perr : "unknown");
                fprintf(stderr, "  >> %s\n", line);
            }
        }

        free(line);
    }

    if (opened_json)
        fprintf(out, "]\n");

    // Summary to stderr keeps stdout clean for pipes/redirection
    fprintf(stderr, "[%s] total=%lld parsed=%lld failed=%lld\n",
            label ? label : "-", total, parsed, failed);
}