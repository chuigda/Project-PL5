#ifndef MINI_SCHEME_DUMP_H
#define MINI_SCHEME_DUMP_H

#include "value.h"
#include "syntax.h"

#ifdef __cplusplus
extern "C" {
#endif

void mscm_sytnax_dump(mscm_syntax_node syntax_node);
void mscm_value_dump(mscm_value value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MINI_SCHEME_DUMP_H */
