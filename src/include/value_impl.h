#ifndef MSCM_VALUE_IMPL_H
#define MSCM_VALUE_IMPL_H

#include "value.h"

typedef struct {
    MSCM_VALUE_COMMON
    mscm_func_def *fndef;
    mscm_scope *scope;
} mscm_function;

mscm_value mscm_make_function(mscm_func_def *fndef,
                              mscm_scope *scope);

void mscm_free_value(mscm_value value);
void mscm_free_value_deep(mscm_value value);

#endif /* MSCM_VALUE_IMPL_H */
