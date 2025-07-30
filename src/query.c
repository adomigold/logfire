/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "query.h"
#include "logstore.h"

static int icasecmp(char a, char b)
{
    if (a >= 'A' && a <= 'Z')
        a += 32;
    if (b >= 'A' && b <= 'Z')
        b += 32;
    return (unsigned char)a - (unsigned char)b;
}

static int str_eq_ci(const char *a, const char *b)
{
    for (; *a && *b; a++, b++)
    {
        if (icasecmp(*a, *b))
            return 0;
    }
    return *a == 0 && *b == 0;
}

// simple wildcard match (* ?), case-insensitive optional
static int wildcard_match(const char *s, const char *pat, int ci)
{
    const char *star = NULL, *ss = NULL;
    while (*s)
    {
        if (*pat == '*')
        {
            star = ++pat;
            ss = s;
        }
        else if (*pat == '?' || (!ci && *pat == *s) || (ci && !icasecmp(*pat, *s)))
        {
            pat++;
            s++;
        }
        else if (star)
        {
            pat = star;
            s = ++ss;
        }
        else
            return 0;
    }
    while (*pat == '*')
        pat++;
    return *pat == 0;
}

// parse ISO 8601 "YYYY-MM-DDTHH:MM:SS" (assume UTC)
static int parse_iso_utc(const char *str, time_t *out)
{
    int Y, M, D, h, m;
    int s;
    if (sscanf(str, "%4d-%2d-%2dT%2d:%2d:%2d", &Y, &M, &D, &h, &m, &s) != 6)
        return 0;
    struct tm tmz;
    memset(&tmz, 0, sizeof(tmz));
    tmz.tm_year = Y - 1900;
    tmz.tm_mon = M - 1;
    tmz.tm_mday = D;
    tmz.tm_hour = h;
    tmz.tm_min = m;
    tmz.tm_sec = s;
// timegm replacement: assume environment is UTC; on Windows use _mkgmtime
#ifdef _WIN32
    *out = _mkgmtime(&tmz);
#else
    *out = timegm(&tmz);
#endif
    return (*out != (time_t)-1);
}

// trim surrounding quotes if present
static void unquote(char *s)
{
    size_t n = strlen(s);
    if (n >= 2 && ((s[0] == '"' && s[n - 1] == '"') || (s[0] == '\'' && s[n - 1] == '\'')))
    {
        memmove(s, s + 1, n - 2);
        s[n - 2] = '\0';
    }
}

// map field name
static int map_field(const char *name, QueryField *out)
{
    if (str_eq_ci(name, "status"))
    {
        *out = QF_STATUS;
        return 1;
    }
    if (str_eq_ci(name, "ip"))
    {
        *out = QF_IP;
        return 1;
    }
    if (str_eq_ci(name, "method"))
    {
        *out = QF_METHOD;
        return 1;
    }
    if (str_eq_ci(name, "url"))
    {
        *out = QF_URL;
        return 1;
    }
    if (str_eq_ci(name, "timestamp"))
    {
        *out = QF_TIMESTAMP;
        return 1;
    }
    if (str_eq_ci(name, "useragent"))
    {
        *out = QF_USERAGENT;
        return 1;
    }
    return 0;
}

// detect operator and split "field<op>value"
static int split_token(char *tok, char *field, char *op, char *val)
{
    // try two-char ops first
    const char *ops2[] = {">=", "<=", "!=", NULL};
    for (int i = 0; ops2[i]; i++)
    {
        char *p = strstr(tok, ops2[i]);
        if (p)
        {
            strncpy(field, tok, p - tok);
            field[p - tok] = 0;
            strcpy(op, ops2[i]);
            strcpy(val, p + 2);
            return 1;
        }
    }
    // one-char ops
    const char *ops1 = ":=><";
    for (const char *q = tok; *q; q++)
    {
        if (strchr(ops1, *q))
        {
            size_t L = (size_t)(q - tok);
            strncpy(field, tok, L);
            field[L] = 0;
            op[0] = *q;
            op[1] = 0;
            strcpy(val, q + 1);
            return 1;
        }
    }
    return 0;
}

// tokenize by spaces, honoring quotes
static int next_token(const char **p, char *buf, size_t bufsz)
{
    const char *s = *p;
    while (*s && isspace((unsigned char)*s))
        s++;
    if (!*s)
    {
        *p = s;
        return 0;
    }

    char *o = buf;
    size_t left = bufsz - 1;
    int q = 0;
    char qc = 0;
    for (; *s && left > 0; s++)
    {
        if (!q && isspace((unsigned char)*s))
            break;
        if (!q && (*s == '"' || *s == '\''))
        {
            q = 1;
            qc = *s;
        }
        else if (q && *s == qc)
        {
            q = 0;
        }
        else
        {
            *o++ = *s;
            left--;
        }
    }
    *o = 0;
    *p = s;
    return 1;
}

// --- public: parse and match ---

int query_parse(const char *expr, int case_insensitive, Query *out, char *errmsg, size_t errmsg_sz)
{
    memset(out, 0, sizeof(*out));
    out->case_insensitive = case_insensitive;

    const char *p = expr;
    char tok[2048];
    while (next_token(&p, tok, sizeof(tok)))
    {
        if (out->count >= (int)(sizeof(out->terms) / sizeof(out->terms[0])))
        {
            snprintf(errmsg, errmsg_sz, "too many terms");
            return 0;
        }
        char field[64], op[3], val[1024];
        if (!split_token(tok, field, op, val))
        {
            snprintf(errmsg, errmsg_sz, "bad token: %s", tok);
            return 0;
        }
        unquote(val);

        QueryTerm *t = &out->terms[out->count];
        if (!map_field(field, &t->field))
        {
            snprintf(errmsg, errmsg_sz, "unknown field: %s", field);
            return 0;
        }

        if (strcmp(op, ":") == 0)
            t->op = QOP_CONTAINS; // string contains / wildcard; numeric == for status
        else if (strcmp(op, "=") == 0)
            t->op = QOP_EQ;
        else if (strcmp(op, "!=") == 0)
            t->op = QOP_NE;
        else if (strcmp(op, ">") == 0)
            t->op = QOP_GT;
        else if (strcmp(op, "<") == 0)
            t->op = QOP_LT;
        else if (strcmp(op, ">=") == 0)
            t->op = QOP_GTE;
        else if (strcmp(op, "<=") == 0)
            t->op = QOP_LTE;
        else
        {
            snprintf(errmsg, errmsg_sz, "bad op: %s", op);
            return 0;
        }

        strncpy(t->value, val, sizeof(t->value) - 1);

        // pre-parse numeric/time
        if (t->field == QF_STATUS)
        {
            t->value_i = atoi(val);
            t->has_i = 1;
        }
        else if (t->field == QF_TIMESTAMP)
        {
            time_t tt;
            if (parse_iso_utc(val, &tt))
            {
                t->value_t = tt;
                t->has_t = 1;
            }
            // else leave as string; you could also support Apache format later
        }
        out->count++;
    }
    return 1;
}

static int cmp_int(int a, QueryOp op, int b)
{
    switch (op)
    {
    case QOP_EQ:
        return a == b;
    case QOP_NE:
        return a != b;
    case QOP_GT:
        return a > b;
    case QOP_LT:
        return a < b;
    case QOP_GTE:
        return a >= b;
    case QOP_LTE:
        return a <= b;
    default:
        return 0;
    }
}
static int cmp_time(time_t a, QueryOp op, time_t b) { return cmp_int((int)a, op, (int)b); }

int query_match(const LogEntry *e, const Query *q)
{
    for (int i = 0; i < q->count; i++)
    {
        const QueryTerm *t = &q->terms[i];
        int ok = 0;
        switch (t->field)
        {
        case QF_STATUS:
        {
            if (t->op == QOP_CONTAINS || !t->has_i)
            { // treat as string contains on decimal
                char buf[16];
                snprintf(buf, sizeof(buf), "%d", e->status);
                ok = wildcard_match(buf, t->value, 1);
            }
            else
            {
                ok = cmp_int(e->status, t->op, t->value_i);
            }
        }
        break;
        case QF_TIMESTAMP:
        {
            if (t->has_t)
                ok = cmp_time(e->epoch, t->op, t->value_t);
            else
                ok = wildcard_match(e->timestamp, t->value, q->case_insensitive);
        }
        break;
        case QF_IP:
            ok = wildcard_match(e->ip, t->value, q->case_insensitive);
            break;
        case QF_METHOD:
            ok = wildcard_match(e->method, t->value, q->case_insensitive);
            break;
        case QF_URL:
            ok = wildcard_match(e->url, t->value, q->case_insensitive);
            break;
        case QF_USERAGENT:
            ok = wildcard_match(e->userAgent, t->value, q->case_insensitive);
            break;
        }
        if (!ok)
            return 0; // AND semantics
    }
    return 1;
}

// Case-insensitive substring search (portable)
static int contains_ci(const char *hay, const char *needle)
{
    if (!hay || !needle)
        return 0;
    size_t hn = strlen(hay), nn = strlen(needle);
    if (nn == 0)
        return 1;
    for (size_t i = 0; i + nn <= hn; i++)
    {
        size_t j = 0;
        while (j < nn)
        {
            unsigned char a = (unsigned char)hay[i + j];
            unsigned char b = (unsigned char)needle[j];
            if (a >= 'A' && a <= 'Z')
                a = (unsigned char)(a + 32);
            if (b >= 'A' && b <= 'Z')
                b = (unsigned char)(b + 32);
            if (a != b)
                break;
            j++;
        }
        if (j == nn)
            return 1;
    }
    return 0;
}

int matches(const LogEntry *e, const char *needle, int case_insensitive)
{
    if (!needle || needle[0] == '\0')
        return 1; // no filter -> match all

    if (case_insensitive)
    {
        return contains_ci(e->method, needle) ||
               contains_ci(e->url, needle) ||
               contains_ci(e->userAgent, needle) ||
               contains_ci(e->timestamp, needle) ||
               contains_ci(e->ip, needle);
    }
    else
    {
        return (e->method && strstr(e->method, needle)) ||
               (e->url && strstr(e->url, needle)) ||
               (e->userAgent && strstr(e->userAgent, needle)) ||
               (e->timestamp && strstr(e->timestamp, needle)) ||
               (e->ip && strstr(e->ip, needle));
    }
}