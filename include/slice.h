#ifndef MINI_SCHEME_SLICE_H
#define MINI_SCHEME_SLICE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *start;
    size_t len;
} mscm_slice;

mscm_slice mscm_slice_from_cstr(char const *cstr);

#define MSCM_NULL_SLICE ((mscm_slice) { 0, 0 })

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MINI_SCHEME_SLICE_H */
