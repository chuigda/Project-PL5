#ifndef MINI_SCHEME_RT_H
#define MINI_SCHEME_RT_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_mscm_value_base *mscm_value;
typedef struct st_mscm_runtime mscm_runtime;

void mscm_runtime_push(mscm_runtime *rt,
                       const char *name,
                       mscm_value value);
void mscm_toggle_gc(mscm_runtime *rt, bool enable);
void mscm_runtime_gc_add(mscm_runtime *rt, mscm_value value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MINI_SCHEME_RT_H */
