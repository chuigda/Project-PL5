#ifndef MINI_SCHEME_RT_H
#define MINI_SCHEME_RT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_mscm_value_base *mscm_value;
typedef struct st_mscm_scope mscm_scope;
typedef struct st_mscm_runtime mscm_runtime;

void mscm_runtime_push(mscm_runtime *rt,
                       const char *name,
                       mscm_value value);
mscm_value mscm_runtime_get(mscm_runtime *rt,
                            const char *name,
                            bool *ok);
void mscm_runtime_trace_exit(mscm_runtime *rt);

void mscm_gc_toggle(mscm_runtime *rt, bool enable);
void mscm_gc_add(mscm_runtime *rt, mscm_value value);
void mscm_gc_mark(mscm_value value);
void mscm_gc_mark_scope(mscm_scope *scope);

uint32_t mscm_runtime_alloc_type_id(mscm_runtime *rt);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MINI_SCHEME_RT_H */
