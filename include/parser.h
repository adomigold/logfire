/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda and contributors
 */
#ifndef PARSER_H
#define PARSER_H

#include "logstore.h"

char *read_line_dyn(FILE *fp);
int parse_apache_or_nginx(const char *line, LogEntry *out, char *errmsg, size_t errmsg_sz);

#endif // PARSER_H
