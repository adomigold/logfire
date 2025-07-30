/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda and contributors
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "logfire.h"
#include "parser.h"
#include "formatter.h"
#include "cli.h"

FILE *out = NULL;

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

    // No inputs? parseCLI already added "-" (stdin)
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
    free((void *)opts.inputs); // we free only the array (not the strings, they point into argv)

    return 0;
}
