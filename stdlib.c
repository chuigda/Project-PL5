#include <stdio.h>

#include "rt.h"
#include "value.h"
#include "dump.h"

static mscm_value display(mscm_runtime *rt,
                          mscm_scope *scope,
                          void *ctx,
                          size_t narg,
                          mscm_value *args);

void mscm_load_ext(mscm_runtime *rt) {
    mscm_value display_fn = mscm_make_native_function(display, 0, 0);
    mscm_runtime_push(rt, "display", (mscm_value)display_fn);
    mscm_runtime_gc_add(rt, display_fn);
}

static mscm_value display(mscm_runtime *rt,
                          mscm_scope *scope,
                          void *ctx,
                          size_t narg,
                          mscm_value *args) {
    (void)rt;
    (void)scope;
    (void)ctx;

    for (size_t i = 0; i < narg; i++) {
        mscm_value_dump(args[i]);
        putchar(' ');
    }
    putchar('\n');
    return 0;
}