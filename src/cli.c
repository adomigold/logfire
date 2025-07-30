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
 * ("json" and "csv") and returns the corresponding OutputFormat enumeration value.
 * If the argument does not match any known format, FORMAT_TEXT is returned by default.
 *
 * @param arg The string argument representing the desired output format.
 * @return OutputFormat The corresponding output format enumeration value.
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
 * @brief Adds an input path to the CLIOptions structure, dynamically resizing the inputs array as needed.
 *
 * This function manages the dynamic allocation and resizing of the inputs array within the CLIOptions structure.
 * If the array is uninitialized, it allocates an initial capacity. If the array is full, it doubles its capacity.
 * The provided path is then added to the array.
 *
 * @param opts Pointer to the CLIOptions structure where the input path will be added.
 * @param path The input path to add to the options.
 * @param cap Pointer to an integer representing the current capacity of the inputs array. This value may be updated if the array is resized.
 */
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
            "Usage: logfire [--log FILE | --log -]... [--search TERM] [--query EXPR]\n"
            "               [--format text|json|csv] [--output FILE]\n"
            "               [--strict] [--ci] [--tail|-f] [--from-start]\n"
            "               [--help]\n"
            "\n"
            "Examples:\n"
            "  gunzip -c access.log.1.gz | logfire --log - --format json > out.json\n"
            "  logfire --log access.log --log access.log.1 --query \"status>=500 ip:10.*\" --format csv\n"
            "  logfire --log access.log --tail -f --query \"method:POST url:*login*\" --format json\n");
}

/**
 * @brief Parse command-line arguments into CLIOptions.
 *
 * Supports:
 *   --log <file>      : Add input (multiple allowed). Use '-' for stdin.
 *   --search <term>   : Keyword search (simple contains across fields).
 *   --query  <expr>   : Field-based query (status, ip, method, url, timestamp, etc.).
 *   --format <type>   : text | json | csv (default: text).
 *   --output <file>   : Write to file (otherwise stdout).
 *   --strict          : Warn/print malformed lines to stderr.
 *   --ci              : Case-insensitive matching.
 *   --tail, -f        : Follow file (tail -f). Use with a single --log file.
 *   --from-start      : With --tail, start at beginning (default: end).
 *   --help, -h        : Show usage.
 *   --                : Treat remaining args as filenames.
 *
 * If no inputs are given, defaults to reading from stdin ("-").
 */
CLIOptions parseCLI(int argc, char *argv[])
{
    CLIOptions opts = {
        .inputs = NULL,
        .input_count = 0,
        .searchTerm = NULL,
        .query = NULL,
        .format = FORMAT_TEXT,
        .outputFile = NULL,
        .strict = 0,
        .case_insensitive = 0,
        .tail = 0,
        .from_start = 0,
    };

    int cap = 0;

    for (int i = 1; i < argc; i++)
    {
        const char *a = argv[i];

        if (strcmp(a, "--") == 0)
        {
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
        else if (strcmp(a, "--query") == 0)
        {
            if (i + 1 >= argc)
            {
                fprintf(stderr, "--query requires an expression\n");
                exit(1);
            }
            opts.query = argv[++i];
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
        else if (strcmp(a, "--tail") == 0 || strcmp(a, "-f") == 0)
        {
            opts.tail = 1;
        }
        else if (strcmp(a, "--from-start") == 0)
        {
            opts.from_start = 1;
        }
        else if (strcmp(a, "--help") == 0 || strcmp(a, "-h") == 0)
        {
            print_usage();
            exit(0);
        }
        else if (a[0] != '-')
        {
            // bare path -> input filename
            add_input(&opts, a, &cap);
        }
        else
        {
            fprintf(stderr, "Unknown argument: %s\n\n", a);
            print_usage();
            exit(1);
        }
    }

    // Default to stdin if no inputs were provided
    if (opts.input_count == 0)
    {
        print_usage();
        exit(1);
    }

    // If both --search and --query are provided, prefer --query but warn
    if (opts.searchTerm && opts.query)
    {
        fprintf(stderr, "[warn] Both --search and --query provided. Using --query and ignoring --search.\n");
    }

    return opts;
}
