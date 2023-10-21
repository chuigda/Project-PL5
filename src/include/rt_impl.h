#ifndef MINI_SCHEME_RT_IMPL_H
#define MINI_SCHEME_RT_IMPL_H

#include <setjmp.h>
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

typedef struct st_managed_scope {
    struct st_managed_scope *next;
    mscm_scope *scope;
} managed_scope;

typedef struct st_stack_trace {
    struct st_stack_trace *next;
    char const *file;
    size_t line;
    mscm_func_def *fndef;
    char const *native_fn_name;
} stack_trace;

#define GC_BUDGET 4096
#define SCOPE_GC_BUDGET 512

typedef struct st_mscm_runtime {
    mscm_scope *global_scope;
    scope_chain_node *scope_chain;
    rooted_group *rooted_groups;

    bool gc_enabled;
    size_t alloc_count;
    size_t scope_alloc_count;
    managed_value *gc_pool;
    managed_scope *scope_pool;

    jmp_buf err_jmp;
    stack_trace *trace;

    uint32_t next_type_id;
} mscm_runtime;

mscm_runtime *runtime_new(void);
void runtime_free(mscm_runtime *rt);
mscm_value runtime_eval_entry(mscm_runtime *rt,
                              mscm_syntax_node node);
mscm_value runtime_eval(mscm_runtime *rt,
                        mscm_syntax_node syntax_node,
                        bool multiple);

#endif /* MINI_SCHEME_RT_IMPL_H */
