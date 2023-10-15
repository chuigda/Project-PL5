#ifndef MINI_SCHEME_SCOPE_IMPL_H
#define MINI_SCHEME_SCOPE_IMPL_H

#include <stddef.h>
#include <stdint.h>

#include "scope.h"

typedef struct st_hash_item {
    struct st_hash_item *next;
    mscm_value value;
    char key[];
} hash_item;

enum { BUCKET_COUNT = 512 };

struct st_mscm_scope {
    mscm_scope *parent;
    size_t refcnt;
    hash_item *buckets[BUCKET_COUNT];
};

#endif /* MINI_SCHEME_SCOPE_IMPL_H */
