#ifndef MINI_SCHEME_UTIL_H
#define MINI_SCHEME_UTIL_H

#include <stddef.h>

#define MALLOC_T(T) (malloc(sizeof(T)))

#define MALLOC_CHK_RET(T, NAME) \
    T *NAME = malloc(sizeof(T)); \
    if (!NAME) { \
        return 0; \
    }

#define MALLOC_CHK_RET_ZEROED(T, NAME) \
    MALLOC_CHK_RET(T, NAME); \
    memset(NAME, 0, sizeof(T));

void err_print(char const *file, size_t line, char const *error);
void err_printf(char const *file, size_t line, char const *fmt, ...);

#endif /* MINI_SCHEME_UTIL_H */
