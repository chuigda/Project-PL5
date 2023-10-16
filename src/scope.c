#include <stdlib.h>
#include <string.h>

#include "scope.h"
#include "scope_impl.h"
#include "util.h"

static void free_hash_chain(hash_item *chain_ptr);
static uint64_t bkdr_khash(const char *str);

mscm_scope *mscm_scope_new(mscm_scope *parent) {
    MALLOC_CHK_RET_ZEROED(mscm_scope, ret);
    ret->parent = parent;
    ret->gc_mark = false;
    return ret;
}

void mscm_scope_free(mscm_scope *scope) {
    for (size_t i = 0; i < BUCKET_COUNT; i++) {
        free_hash_chain(scope->buckets[i]);
    }
    free(scope);
}

void mscm_scope_push(mscm_scope *scope,
                     const char *name,
                     mscm_value value) {
    size_t size = strlen(name);
    uint64_t hash = bkdr_khash(name);
    size_t bucket = hash % BUCKET_COUNT;
    hash_item *chain = scope->buckets[bucket];

    while (chain) {
        if (!strcmp(chain->key, name)) {
            chain->value = value;
            return;
        }
        chain = chain->next;
    }

    hash_item *item = malloc(sizeof(hash_item) + size + 1);
    if (!item) {
        return;
    }
    item->next = scope->buckets[bucket];
    item->value = value;
    strcpy(item->key, name);
    scope->buckets[bucket] = item;
}

void mscm_scope_set(mscm_scope *scope,
                    const char *name,
                    mscm_value value,
                    bool *ok) {
    uint64_t hash = bkdr_khash(name);
    size_t bucket = hash % BUCKET_COUNT;
    hash_item *chain = scope->buckets[bucket];

    while (chain) {
        if (!strcmp(chain->key, name)) {
            *ok = true;
            chain->value = value;
            return;
        }
        chain = chain->next;
    }

    if (scope->parent) {
        mscm_scope_set(scope->parent, name, value, ok);
    }
    else {
        *ok = false;
    }
}

mscm_value mscm_scope_get(mscm_scope *scope,
                          const char *name,
                          bool *ok) {
    while (scope) {
        mscm_value ret = mscm_scope_get_current(scope, name, ok);
        if (*ok) {
            return ret;
        }

        scope = scope->parent;
    }

    *ok = false;
    return 0;
}

mscm_value mscm_scope_get_current(mscm_scope *scope,
                                  const char *name,
                                  bool *ok) {
    uint64_t hash = bkdr_khash(name);
    size_t bucket = hash % BUCKET_COUNT;
    hash_item *chain = scope->buckets[bucket];

    while (chain) {
        if (!strcmp(chain->key, name)) {
            *ok = true;
            return chain->value;
        }
        chain = chain->next;
    }

    *ok = false;
    return 0;
}

static void free_hash_chain(hash_item *chain_ptr) {
    while (chain_ptr) {
        hash_item *current = chain_ptr;
        chain_ptr = chain_ptr->next;
        free(current);
    }
}

static uint64_t bkdr_khash(const char *str) {
    uint64_t khash = 0;
    char ch = 0;
    ch = *str++;
    while (ch) {
        khash = khash * 13 + ch;
        ch = *str++;
    }
    return khash;
}
