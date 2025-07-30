#include <string.h>
#include "query.h"
#include <logstore.h>

/**
 * @brief Checks if the string `h` contains the substring `n`, case-insensitively.
 *
 * This function performs a case-insensitive search to determine whether the string
 * `h` contains the substring `n`. If either `h` or `n` is NULL, the function returns 0.
 * If `n` is an empty string, the function returns 1.
 *
 * @param h The string to be searched (haystack).
 * @param n The substring to search for (needle).
 * @return int Returns 1 if `h` contains `n` (case-insensitive), 0 otherwise.
 */
static int contains_ci(const char *h, const char *n)
{
    if (!h || !n)
        return 0;
    size_t hl = strlen(h), nl = strlen(n);
    if (nl == 0)
        return 1;
    for (size_t i = 0; i + nl <= hl; i++)
    {
        size_t j = 0;
        while (j < nl)
        {
            unsigned char a = (unsigned char)h[i + j];
            unsigned char b = (unsigned char)n[j];
            if (a >= 'A' && a <= 'Z')
                a += 32;
            if (b >= 'A' && b <= 'Z')
                b += 32;
            if (a != b)
                break;
            j++;
        }
        if (j == nl)
            return 1;
    }
    return 0;
}

/**
 * Checks if any field of the given LogEntry contains the specified needle string.
 *
 * @param e      Pointer to the LogEntry structure to search within.
 * @param needle The substring to search for in the LogEntry fields.
 * @param ci     If non-zero, the search is case-insensitive; otherwise, it is case-sensitive.
 *
 * @return 1 if the needle is found in any of the LogEntry fields (method, url, userAgent, timestamp, ip),
 *         or if needle is NULL or empty; 0 otherwise.
 */
int matches(const LogEntry *e, const char *needle, int ci)
{
    if (!needle || !*needle)
        return 1;
    if (ci)
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
