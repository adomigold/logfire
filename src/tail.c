/*
 * SPDX-License-Identifier: AGPL-3.0-or-later
 */
#define _FILE_OFFSET_BITS 64
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#ifdef _WIN32
#include <windows.h>
static void msleep(int ms) { Sleep(ms); }
#else
#include <unistd.h>
static void msleep(int ms) { usleep(ms * 1000); }
#endif

#include "cli.h"
#include "logstore.h"
#include "query.h"
#include "parser.h"
#include "formatter.h"

char *read_line_dyn(FILE *fp);

static int stat_inode(const char *path, dev_t *dev, ino_t *ino, off_t *size)
{
    struct stat st;
    if (stat(path, &st) != 0)
        return 0;
    if (dev)
        *dev = st.st_dev;
    if (ino)
        *ino = st.st_ino;
    if (size)
        *size = st.st_size;
    return 1;
}

/**
 * @brief Continuously tails a log file, optionally filtering and formatting output.
 *
 * This function opens the specified log file and continuously reads new lines as they are appended,
 * similar to the Unix `tail -f` command. It supports filtering log entries using a query or search term,
 * and can output results in different formats (text, JSON, CSV). The function also handles log rotation
 * by detecting file truncation or inode changes and reopening the file as needed.
 *
 * @param path         Path to the log file to tail.
 * @param from_start   If non-zero, start reading from the beginning of the file; otherwise, start from the end.
 * @param opt          Pointer to CLIOptions structure containing user options (query, search term, format, etc.).
 * @param out          Output stream to write matching log entries.
 *
 * The function will print warnings to stderr if parsing fails and the 'strict' option is enabled.
 * It uses helper functions for parsing log lines, matching queries, and formatting output.
 */
void tail_file(const char *path, int from_start, const CLIOptions *opt, FILE *out)
{
    FILE *fp = fopen(path, "rb");
    if (!fp)
    {
        perror(path);
        return;
    }

    dev_t cur_dev = 0;
    ino_t cur_ino = 0;
    off_t cur_size = 0;
    stat_inode(path, &cur_dev, &cur_ino, &cur_size);

    if (!from_start)
        fseeko(fp, 0, SEEK_END);

    Query q;
    int use_q = 0;
    char qerr[128] = {0};
    if (opt->query && *opt->query)
    {
        if (query_parse(opt->query, opt->case_insensitive, &q, qerr, sizeof(qerr)))
        {
            use_q = 1;
        }
        else
        {
            fprintf(stderr, "query parse error: %s\n", qerr);
        }
    }

    for (;;)
    {
        long pos_before = ftello(fp);
        char *line = read_line_dyn(fp);

        if (!line)
        {
            dev_t new_dev = 0;
            ino_t new_ino = 0;
            off_t new_size = 0;
            if (stat_inode(path, &new_dev, &new_ino, &new_size))
            {
                if (new_size < pos_before)
                {
                    fclose(fp);
                    fp = fopen(path, "rb");
                    if (!fp)
                    {
                        msleep(250);
                        continue;
                    }
                    if (!from_start)
                        fseeko(fp, 0, SEEK_END);
                    cur_dev = new_dev;
                    cur_ino = new_ino;
                    cur_size = new_size;
                }
                else if (new_ino != cur_ino || new_dev != cur_dev)
                {
                    fclose(fp);
                    fp = fopen(path, "rb");
                    if (!fp)
                    {
                        msleep(250);
                        continue;
                    }
                    if (!from_start)
                        fseeko(fp, 0, SEEK_END);
                    cur_dev = new_dev;
                    cur_ino = new_ino;
                    cur_size = new_size;
                }
            }
            msleep(200);
            continue;
        }

        LogEntry e;
        char perr[256] = {0};
        if (parse_apache_or_nginx(line, &e, perr, sizeof(perr)))
        {
            int ok = 1;
            if (use_q)
                ok = query_match(&e, &q);
            else if (opt->searchTerm && *opt->searchTerm)
                ok = matches(&e, opt->searchTerm, opt->case_insensitive);

            if (ok)
            {
                if (opt->format == FORMAT_JSON)
                {
                    printLogJSON(&e, out);
                    fputc('\n', out);
                }
                else if (opt->format == FORMAT_CSV)
                {
                    printLogCSV(&e, out);
                    fputc('\n', out);
                }
                else
                {
                    printLogText(&e, out);
                    fputc('\n', out);
                }
            }
        }
        else if (opt->strict)
        {
            fprintf(stderr, "[tail warn] %s\n", perr[0] ? perr : "parse failed");
            fprintf(stderr, "  >> %s\n", line);
        }

        free(line);
    }

    fclose(fp);
}
