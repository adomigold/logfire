/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda and contributors
 */
#ifndef CLI_H
#define CLI_H

typedef enum
{
    FORMAT_TEXT,
    FORMAT_JSON,
    FORMAT_CSV
} OutputFormat;

typedef struct
{
    const char *logfile;
    const char *searchTerm;
    OutputFormat format;
    const char *outputFile; // Optional output file for redirection
} CLIOptions;

CLIOptions parseCLI(int argc, char *argv[]);

#endif

