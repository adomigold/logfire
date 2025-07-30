/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda and contributors
 */
#ifndef LOGFIRE_H
#define LOGFIRE_H
#include "parser.h"
#include "formatter.h"
#include "logstore.h"
#include "cli.h"

extern enum OutputFormat currentFormat;
void process_stream(FILE *in, const char *label, const CLIOptions *opt, FILE *out);

#endif // LOGFIRE_H
