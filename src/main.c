/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda and contributors
 */
#include <stdio.h>
#include <string.h>
#include "logfire.h"
#include "parser.h"
#include "formatter.h"
#include "cli.h"

FILE *out = NULL;

void parseLogFile(const char *filename);
void printFormatted(LogEntry *entry, enum OutputFormat fmt, FILE *out);

int main(int argc, char *argv[])
{
    CLIOptions options = parseCLI(argc, argv);

    FILE *out = stdout;
    if (options.outputFile)
    {
        out = fopen(options.outputFile, "w");
        if (!out)
        {
            perror("Failed to open output file");
            return 1;
        }
    }

    parseLogFile(options.logfile);
    searchLogs(options.searchTerm, options.format, out);

    if (out != stdout)
    {
        fclose(out);
    }

    return 0;
}

void printFormatted(LogEntry *entry, enum OutputFormat fmt, FILE *out)
{
    switch (fmt)
    {
    case JSON:
        printLogJSON(entry, out);
        break;
    case CSV:
        printLogCSV(entry, out);
        break;
    default:
        printLogText(entry, out);
        break;
    }
}

