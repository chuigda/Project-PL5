#include <stdlib.h>
#include <string.h>

#include "scope_impl.h"
#include "util.h"

static void free_hash_chain(hash_item *chain_ptr);
static uint64_t bkdr_khash(const char *str);

mscm_scope *mscm_scope_new(mscm_scope *parent) {
    MALLOC_CHK_RET_ZEROED(mscm_scope, ret);
    ret->parent = parent;
    ret->refcnt = 1;
    return ret;
}

void mscm_scope_incref(mscm_scope *scope) {
    scope->refcnt += 1;
}

void mscm_scope_decref(mscm_scope *scope) {
    if (scope->refcnt > 0) {
        scope->refcnt -= 1;
    }

    if (scope->refcnt != 0) {
        return;
    }

    for (size_t i = 0; i < BUCKET_COUNT; i++) {
        free_hash_chain(scope->buckets[i]);
    }

    mscm_scope *parent = scope->parent;
    free(scope);

    if (parent) {
        mscm_scope_decref(parent);
    }
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
    item->next = chain;
    item->value = value;
    strcpy(item->key, name);
    scope->buckets[bucket] = item;
}

mscm_value mscm_scope_get(mscm_scope *scope,
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

    if (scope->parent) {
        return mscm_scope_get(scope->parent, name, ok);
    } else {
        *ok = false;
        return 0;
    }
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
