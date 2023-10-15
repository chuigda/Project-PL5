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

struct st_mscm_scope {
    mscm_scope *parent;
    hash_item *buckets[BUCKET_COUNT];
    bool gc_mark;
};

void mscm_scope_free(mscm_scope *scope);

#endif /* MINI_SCHEME_SCOPE_IMPL_H */
