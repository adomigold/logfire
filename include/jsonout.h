/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda
 */
#ifndef JSONOUT_H
#define JSONOUT_H
#include <stdio.h>

typedef struct
{
    int first;
} JsonArrayCtx;

static inline void json_array_begin(FILE *out, JsonArrayCtx *ctx)
{
    ctx->first = 1;
    fprintf(out, "[");
}
static inline void json_array_sep(FILE *out, JsonArrayCtx *ctx)
{
    if (!ctx->first)
        fprintf(out, ",");
    ctx->first = 0;
}
static inline void json_array_end(FILE *out) { fprintf(out, "]\n"); }

#endif // JSONOUT_H