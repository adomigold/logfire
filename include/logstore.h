/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 * Copyright (c) 2025 Adolph Mapunda and contributors
 */
#ifndef LOGSTORE_H
#define LOGSTORE_H

typedef struct
{
    char timestamp[64];
    char ip[64];
    char method[16];
    char url[1024];
    int status;
    char userAgent[1024];
} LogEntry;

#endif // LOGSTORE_H
