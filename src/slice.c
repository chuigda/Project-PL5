#include <string.h>
#include "slice.h"

mscm_slice mscm_slice_from_cstr(char const *cstr) {
    return (mscm_slice) { cstr, strlen(cstr) };
}
