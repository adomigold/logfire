/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda and contributors
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"

/**
 * @brief Parses a string argument to determine the output format.
 *
 * This function compares the input string argument with known format strings
 * ("json" and "csv") and returns the corresponding OutputFormat enum value.
 * If the argument does not match any known format, FORMAT_TEXT is returned as the default.
 *
 * @param arg The string argument representing the desired output format.
 * @return OutputFormat The corresponding output format enum value.
 */
static OutputFormat parseFormatArg(const char *arg)
{
    if (strcmp(arg, "json") == 0)
        return FORMAT_JSON;
    if (strcmp(arg, "csv") == 0)
        return FORMAT_CSV;
    return FORMAT_TEXT;
}

/**
 * @brief Parses command-line arguments and returns a CLIOptions struct.
 *
 * This function processes the command-line arguments provided to the program,
 * extracting options such as the log file path, search term, output format,
 * and output file. It supports the following arguments:
 *   --log <file>      : Specifies the log file to process (required).
 *   --search <term>   : Specifies a search term to filter log entries (optional).
 *   --format <type>   : Specifies the output format: text, json, or csv (optional, defaults to text).
 *   --output <file>   : Specifies a file to write the output to (optional).
 *   --help            : Displays usage information and exits.
 *
 * If an unknown argument is encountered or a required argument is missing,
 * the function prints an error message and exits the program.
 *
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line argument strings.
 * @return CLIOptions struct populated with the parsed options.
 */
CLIOptions parseCLI(int argc, char *argv[])
{
    CLIOptions opts = {
        .logfile = NULL,
        .searchTerm = NULL,
        .format = FORMAT_TEXT,
        .outputFile = NULL};

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--log") == 0 && i + 1 < argc)
        {
            opts.logfile = argv[++i];
        }
        else if (strcmp(argv[i], "--search") == 0 && i + 1 < argc)
        {
            opts.searchTerm = argv[++i];
        }
        else if (strcmp(argv[i], "--format") == 0 && i + 1 < argc)
        {
            opts.format = parseFormatArg(argv[++i]);
        }
        else if (strcmp(argv[i], "--output") == 0 && i + 1 < argc)
        {
            opts.outputFile = argv[++i];
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            printf("Usage: logfire --log <file> [--search <term>] [--format text|json|csv]\n");
            exit(0);
        }
        else
        {
            printf("Unknown argument: %s\n", argv[i]);
            exit(1);
        }
    }

    if (!opts.logfile)
    {
        fprintf(stderr, "Error: --log <file> is required.\n");
        exit(1);
    }

    return opts;
}
