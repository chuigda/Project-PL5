#ifndef MINI_SCHEME_SCOPE_IMPL_H
#define MINI_SCHEME_SCOPE_IMPL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "scope.h"

typedef struct st_hash_item {
    struct st_hash_item *next;
    mscm_value value;
    char key[];
} hash_item;

enum { BUCKET_COUNT = 32 };

#define SCOPE_COMMON \
    mscm_scope *parent; \
    bool gc_mark; \
    bool is_fat;

struct st_mscm_scope {
    SCOPE_COMMON
};

typedef struct st_mscm_scope_thin {
    SCOPE_COMMON
    hash_item *list;
} mscm_scope_thin;

typedef struct st_mscm_scope_fat {
    SCOPE_COMMON
    hash_item *buckets[BUCKET_COUNT];
} mscm_scope_fat;

void mscm_scope_free(mscm_scope *scope);

#endif /* MINI_SCHEME_SCOPE_IMPL_H */
