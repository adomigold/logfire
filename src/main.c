/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"

// Prototypes from your other modules
void process_stream(FILE *in, const char *label, const CLIOptions *opt, FILE *out);
void tail_file(const char *path, int from_start, const CLIOptions *opt, FILE *out);

int main(int argc, char *argv[])
{
    CLIOptions opts = parseCLI(argc, argv);

    FILE *out = stdout;
    if (opts.outputFile)
    {
        out = fopen(opts.outputFile, "w");
        if (!out)
        {
            perror("open output");
            return 1;
        }
    }
    
    if (opts.tail)
    {
        
        if (opts.input_count != 1)
        {
            fprintf(stderr, "Error: --tail expects exactly one --log FILE (not multiple, not stdin).\n");
            if (out != stdout)
                fclose(out);
            free((void *)opts.inputs);
            return 1;
        }
        const char *path = opts.inputs[0];
        if (strcmp(path, "-") == 0)
        {
            fprintf(stderr, "Error: --tail cannot follow stdin. Provide a file path with --log.\n");
            if (out != stdout)
                fclose(out);
            free((void *)opts.inputs);
            return 1;
        }
        // Tail mode: stream indefinitely; recommend NDJSON for JSON output in tail_file
        tail_file(path, opts.from_start, &opts, out);

        if (out != stdout)
            fclose(out);
        free((void *)opts.inputs);
        return 0;
    }

    for (int i = 0; i < opts.input_count; i++)
    {
        const char *path = opts.inputs[i];
        if (strcmp(path, "-") == 0)
        {
            process_stream(stdin, "-", &opts, out);
        }
        else
        {
            FILE *fp = fopen(path, "rb");
            if (!fp)
            {
                perror(path);
                continue;
            }
            process_stream(fp, path, &opts, out);
            fclose(fp);
        }
    }

    if (out != stdout)
        fclose(out);
    free((void *)opts.inputs); // only the array; entries point to argv

    return 0;
}
