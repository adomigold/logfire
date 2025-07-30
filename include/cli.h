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
    const char **inputs; // array of input paths; we use "-" for stdin
    int input_count;
    const char *searchTerm; // may be NULL or ""
    OutputFormat format;    // FORMAT_TEXT / FORMAT_JSON / FORMAT_CSV
    const char *outputFile; // nullable
    int strict;             // print parse errors/unknown lines to stderr
    int case_insensitive;   // case-insensitive search
} CLIOptions;

CLIOptions parseCLI(int argc, char *argv[]);

// Small helper to parse "text|json|csv"
static OutputFormat parseFormatArg(const char *arg);

#endif
