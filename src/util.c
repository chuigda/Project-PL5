#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>

#include "util.h"

void err_print(char const *file, size_t line, char const *error) {
    fprintf(stderr,
            "error: %s:%" PRIu64 ": %s\n",
            file ? file : "(unknown)", (uint64_t)line, error);

}

void err_printf(char const *file, size_t line, char const *fmt, ...) {
    fprintf(stderr,
            "error: %s:%" PRIu64 ": ",
            file ? file : "(unknown)", (uint64_t)line);

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
}
