#include <stdio.h>

#include "rt.h"
#include "value.h"
#include "dump.h"

#define MSCM_NATIVE_FN_PARAM \
    mscm_runtime *rt, \
    mscm_scope *scope, \
    void *ctx, \
    size_t narg, \
    mscm_value *args

#define MSCM_NATIVE_FN(FNAME) \
    static mscm_value FNAME(MSCM_NATIVE_FN_PARAM)

MSCM_NATIVE_FN(display);
MSCM_NATIVE_FN(error);

void mscm_load_ext(mscm_runtime *rt) {
    mscm_value display_v = mscm_make_native_function(display, 0, 0);
    mscm_value error_v = mscm_make_native_function(error, 0, 0);

    mscm_runtime_push(rt, "display", (mscm_value)display_v);
    mscm_runtime_push(rt, "error", (mscm_value)error_v);
    mscm_runtime_gc_add(rt, display_v);
    mscm_runtime_gc_add(rt, error_v);
}

MSCM_NATIVE_FN(display) {
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

MSCM_NATIVE_FN(error) {
    (void)scope;
    (void)ctx;

    fprintf(stderr, "error: ");
    for (size_t i = 0; i < narg; i++) {
        if (args[i]->type == MSCM_TYPE_STRING) {
            mscm_string *s = (mscm_string*)args[i];
            fprintf(stderr, "%s", s->buf);
        }
        fputc(' ', stderr);
    }
    putchar('\n');
    
    mscm_runtime_trace_exit(rt);
    return 0;
}
