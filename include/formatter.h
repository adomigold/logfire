/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda and contributors
 */
#ifndef FORMATTER_H
#define FORMATTER_H
#pragma once
#include "logstore.h"

enum OutputFormat
{
    TEXT,
    JSON,
    CSV
};

void printLogText(LogEntry *entry, FILE *out);
void printLogJSON(LogEntry *entry, FILE *out);
void printLogCSV(LogEntry *entry, FILE *out);
void printFormatted(LogEntry *entry, enum OutputFormat fmt, FILE *out);
 
#endif // FORMATTER_H
