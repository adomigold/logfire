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

// Append a path to opts->inputs, growing the array as needed
static void add_input(CLIOptions *opts, const char *path, int *cap)
{
    if (opts->inputs == NULL)
    {
        *cap = 4;
        opts->inputs = (const char **)malloc((*cap) * sizeof(char *));

        if (!opts->inputs)
        {
            perror("malloc");
            exit(1);
        }
    }

    // Check if we need to grow the array
    if (opts->input_count >= *cap)
    {
        *cap *= 2;
        const char **tmp = (const char **)realloc((void *)opts->inputs, (*cap) * sizeof(char *));
        if (!tmp)
        {
            perror("realloc");
            exit(1);
        }
        opts->inputs = tmp;
    }
    opts->inputs[opts->input_count++] = path;
}

static void print_usage(void)
{
    fprintf(stderr,
            "Usage: logfire [--log FILE | --log -]... [--search TERM]\n"
            "               [--format text|json|csv] [--output FILE]\n"
            "               [--strict] [--ci] [--help]\n"
            "\n"
            "Examples:\n"
            "  gunzip -c access.log.1.gz | logfire --log - --format json > out.json\n"
            "  logfire --log access.log --log access.log.1 --search 500 --format csv\n");
}

/**
 * @brief Parses command-line arguments and returns a CLIOptions struct.
 *
 * Processes command-line arguments for the logfire program, supporting:
 *   --log <file>      : Add a log file to process (can be used multiple times, or use "-" for stdin).
 *   --search <term>   : Filter log entries by a search term (optional).
 *   --format <type>   : Output format: text, json, or csv (optional, defaults to text).
 *   --output <file>   : Write output to a file (optional).
 *   --strict          : Enable strict mode (optional).
 *   --ci, --case-insensitive : Enable case-insensitive search (optional).
 *   --help, -h        : Show usage information and exit.
 *   --                : Treat all following arguments as input files.
 *   <file>            : Bare arguments are treated as input files.
 *
 * If no input files are specified, stdin ("-") is used by default.
 * On error or unknown argument, prints a message and exits.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return CLIOptions struct with parsed options.
 */
CLIOptions parseCLI(int argc, char *argv[])
{
    CLIOptions opts = {
        .inputs = NULL,
        .searchTerm = NULL,
        .input_count = 0,
        .format = FORMAT_TEXT,
        .outputFile = NULL,
        .strict = 0,
        .case_insensitive = 0,
    };

    int cap = 0;

    for (int i = 1; i < argc; i++)
    {
        const char *a = argv[i];

        if (strcmp(a, "--") == 0)
        { // stop option parsing; rest are files
            for (int j = i + 1; j < argc; j++)
                add_input(&opts, argv[j], &cap);
            break;
        }
        else if (strcmp(a, "--log") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "--log requires a path (or - for stdin)\n");
                exit(1);
            }
            add_input(&opts, argv[++i], &cap);
        }
        else if (strcmp(a, "--search") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "--search requires a term\n");
                exit(1);
            }
            opts.searchTerm = argv[++i];
        }
        else if (strcmp(a, "--format") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "--format requires text|json|csv\n");
                exit(1);
            }
            opts.format = parseFormatArg(argv[++i]);
        }
        else if (strcmp(a, "--output") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "--output requires a filename\n");
                exit(1);
            }
            opts.outputFile = argv[++i];
        }
        else if (strcmp(a, "--strict") == 0)
        {
            opts.strict = 1;
        }
        else if (strcmp(a, "--ci") == 0 || strcmp(a, "--case-insensitive") == 0)
        {
            opts.case_insensitive = 1;
        }
        else if (strcmp(a, "--help") == 0 || strcmp(a, "-h") == 0)
        {
            print_usage();
            exit(0);
        }
        else if (a[0] != '-')
        {
            // treat bare argument as an input filename for convenience
            add_input(&opts, a, &cap);
        }
        else
        {
            fprintf(stderr, "Unknown argument: %s\n\n", a);
            print_usage();
            exit(1);
        }
    }

    if (opts.input_count == 0)
    {
        print_usage();
        exit(1);
    }
    return opts;
}