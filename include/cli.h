/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda
 */
#ifndef CLI_H
#define CLI_H

#include <stdio.h>

typedef enum
{
    FORMAT_TEXT,
    FORMAT_JSON,
    FORMAT_CSV
} OutputFormat;

typedef struct
{
    const char **inputs;
    int input_count;
    const char *searchTerm;
    const char *query;
    OutputFormat format;
    const char *outputFile;
    int strict;
    int case_insensitive;
    int tail;
    int from_start;
} CLIOptions;

CLIOptions parseCLI(int argc, char *argv[]);

// Small helper to parse "text|json|csv"
static OutputFormat parseFormatArg(const char *arg);

#endif
