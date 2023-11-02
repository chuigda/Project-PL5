#include <stdlib.h>
#include <string.h>

#include "scope.h"
#include "scope_impl.h"
#include "util.h"

static hash_item* push_hash_chain(hash_item *chain,
                                  char const *name,
                                  mscm_value value);
static bool set_hash_chain(hash_item *chain,
                           char const *name,
                           mscm_value value);
static mscm_value get_hash_chain(hash_item *chain,
                                 char const *name,
                                 bool *ok);
static void free_hash_chain(hash_item *chain);
static uint64_t bkdr_khash(const char *str);

mscm_scope *mscm_scope_new(mscm_scope *parent, bool fat) {
    if (fat) {
        MALLOC_CHK_RET_ZEROED(mscm_scope_fat, ret);
        ret->parent = parent;
        ret->gc_mark = false;
        ret->is_fat = true;
        return (mscm_scope*)ret;
    }
    else {
        MALLOC_CHK_RET_ZEROED(mscm_scope_thin, ret);
        ret->parent = parent;
        ret->gc_mark = false;
        ret->is_fat = false;
        return (mscm_scope*)ret;
    }
}

void mscm_scope_free(mscm_scope *scope) {
    if (scope->is_fat) {
        mscm_scope_fat *scope_fat = (mscm_scope_fat*)scope;
        for (size_t i = 0; i < BUCKET_COUNT; i++) {
            free_hash_chain(scope_fat->buckets[i]);
        }
    }
    else {
        mscm_scope_thin *scope_thin = (mscm_scope_thin*)scope;
        free_hash_chain(scope_thin->list);
    }
    free(scope);
}

void mscm_scope_push(mscm_scope *scope,
                     const char *name,
                     mscm_value value) {
    if (scope->is_fat) {
        mscm_scope_fat *scope_fat = (mscm_scope_fat*)scope;
        uint64_t hash = bkdr_khash(name);
        size_t bucket = hash % BUCKET_COUNT;
        hash_item *chain = scope_fat->buckets[bucket];
        hash_item *new_item = push_hash_chain(chain, name, value);
        if (new_item) {
            new_item->next = chain;
            scope_fat->buckets[bucket] = new_item;
        }
    } else {
        mscm_scope_thin *scope_thin = (mscm_scope_thin*)scope;
        hash_item *new_item = push_hash_chain(scope_thin->list,
                                              name,
                                              value);
        if (new_item) {
            new_item->next = scope_thin->list;
            scope_thin->list = new_item;
        }
    }
}

void mscm_scope_set(mscm_scope *scope,
                    const char *name,
                    mscm_value value,
                    bool *ok) {
    *ok = false;
    while (scope && !*ok) {
        if (scope->is_fat) {
            mscm_scope_fat *scope_fat = (mscm_scope_fat*)scope;
            uint64_t hash = bkdr_khash(name);
            size_t bucket = hash % BUCKET_COUNT;
            hash_item *chain = scope_fat->buckets[bucket];
            *ok = set_hash_chain(chain, name, value);
        } else {
            mscm_scope_thin *scope_thin = (mscm_scope_thin*)scope;
            *ok = set_hash_chain(scope_thin->list, name, value);
        }

        scope = scope->parent;
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
    if (scope->is_fat) {
        mscm_scope_fat *scope_fat = (mscm_scope_fat*)scope;
        uint64_t hash = bkdr_khash(name);
        size_t bucket = hash % BUCKET_COUNT;
        hash_item *chain = scope_fat->buckets[bucket];
        return get_hash_chain(chain, name, ok);
    } else {
        mscm_scope_thin *scope_thin = (mscm_scope_thin*)scope;
        return get_hash_chain(scope_thin->list, name, ok);
    }
}

static mscm_value get_hash_chain(hash_item *chain,
                                 char const *name,
                                 bool *ok) {
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

static hash_item* push_hash_chain(hash_item *chain,
                                  char const *name,
                                  mscm_value value) {
    size_t size = strlen(name);
    while (chain) {
        if (!strcmp(chain->key, name)) {
            chain->value = value;
            return 0;
        }
        chain = chain->next;
    }

    hash_item *item = malloc(sizeof(hash_item) + size + 1);
    if (!item) {
        return 0;
    }
    item->value = value;

    strcpy(item->key, name);
    return item;
}

static bool set_hash_chain(hash_item *chain,
                           char const *name,
                           mscm_value value) {
    while (chain) {
        if (!strcmp(chain->key, name)) {
            chain->value = value;
            return true;
        }
        chain = chain->next;
    }

    return false;
}

static void free_hash_chain(hash_item *chain) {
    while (chain) {
        hash_item *current = chain;
        chain = chain->next;
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
