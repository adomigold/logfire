/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda
 */
#ifndef QUERY_H
#define QUERY_H
#include "logstore.h"

typedef enum
{
    QF_STATUS,
    QF_IP,
    QF_METHOD,
    QF_URL,
    QF_TIMESTAMP,
    QF_USERAGENT
} QueryField;

typedef enum
{
    QOP_EQ,
    QOP_NE,
    QOP_GT,
    QOP_LT,
    QOP_GTE,
    QOP_LTE,
    QOP_CONTAINS // ':' maps to EQ for numeric, CONTAINS/wildcard for strings
} QueryOp;

typedef struct
{
    QueryField field;
    QueryOp op;
    char value[1024]; // raw value (string); for status/timestamp we also pre-parse
    int value_i;      // numeric (status) if applicable
    time_t value_t;   // timestamp if applicable
    int has_i;
    int has_t;
} QueryTerm;

typedef struct
{
    QueryTerm terms[16]; // up to 16 AND terms v1
    int count;
    int case_insensitive;
} Query;

int query_parse(const char *expr, int case_insensitive, Query *out, char *errmsg, size_t errmsg_sz);
int query_match(const LogEntry *e, const Query *q);
int matches(const LogEntry *e, const char *needle, int case_insensitive);

#endif
