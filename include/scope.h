#ifndef MINI_SCHEME_SCOPE_H
#define MINI_SCHEME_SCOPE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_mscm_value_base *mscm_value;
typedef struct st_mscm_scope mscm_scope;

mscm_scope *mscm_scope_new(mscm_scope *parent);

void mscm_scope_push(mscm_scope *scope,
                     const char *name,
                     mscm_value value);
mscm_value mscm_scope_get(mscm_scope *scope, const char *name, bool *ok);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MINI_SCHEME_SCOPE_H */
