#ifndef MINI_SCHEME_RT_IMPL_H
#define MINI_SCHEME_RT_IMPL_H

#include <stdbool.h>
#include <stddef.h>

#include "scope.h"
#include "syntax.h"
#include "value.h"

typedef struct st_scope_chain_node {
    struct st_scope_chain_node *parent;
    mscm_scope *chain;
} scope_chain_node;

typedef struct st_rooted_value {
    struct st_rooted_value *next;
    mscm_value value;
} rooted_value;

typedef struct st_rooted_group {
    struct st_rooted_group *next;
    rooted_value *values;
} rooted_group;

typedef struct st_managed_value {
    struct st_managed_value *next;
    mscm_value value;
} managed_value;

typedef struct st_stack_trace {
    struct st_stack_trace *next;
    char const *file;
    size_t line;
    mscm_func_def *fndef;
} stack_trace;

#define GC_POOL_BUCKET_SIZE 4096

typedef struct st_mscm_runtime {
    mscm_scope *global_scope;
    scope_chain_node *scope_chain;
    rooted_group *rooted_groups;

    bool gc_enabled;
    size_t alloc_count;
    managed_value *gc_pool_buckets[GC_POOL_BUCKET_SIZE];

    stack_trace *trace;
} mscm_runtime;

mscm_runtime *runtime_new(void);
void runtime_free(mscm_runtime *rt);
mscm_value runtime_eval(mscm_runtime *rt,
                        mscm_syntax_node syntax_node,
                        bool multiple);

#endif /* MINI_SCHEME_RT_IMPL_H */
