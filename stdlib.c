#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "rt.h"
#include "slice.h"
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
MSCM_NATIVE_FN(equals);
MSCM_NATIVE_FN(less_than);
MSCM_NATIVE_FN(add);
MSCM_NATIVE_FN(sub);
MSCM_NATIVE_FN(mul);

static mscm_value g_true_v;

void mscm_load_ext(mscm_runtime *rt) {
    g_true_v = mscm_make_string(mscm_slice_from_cstr("true"), false, 0);
    g_true_v->type = MSCM_TYPE_SYMBOL;
    mscm_runtime_push(rt, "true", (mscm_value)g_true_v);
    mscm_runtime_gc_add(rt, (mscm_value)g_true_v);

    mscm_value display_v = mscm_make_native_function(display, 0, 0);
    mscm_value error_v = mscm_make_native_function(error, 0, 0);
    mscm_value equals_v = mscm_make_native_function(equals, 0, 0);
    mscm_value less_than_v = mscm_make_native_function(less_than, 0, 0);
    mscm_value add_v = mscm_make_native_function(add, 0, 0);
    mscm_value sub_v = mscm_make_native_function(sub, 0, 0);
    mscm_value mul_v = mscm_make_native_function(mul, 0, 0);

    mscm_runtime_push(rt, "display", (mscm_value)display_v);
    mscm_runtime_push(rt, "error", (mscm_value)error_v);
    mscm_runtime_push(rt, "equals?", (mscm_value)equals_v);
    mscm_runtime_push(rt, "=", (mscm_value)equals_v);
    mscm_runtime_push(rt, "less-than?", (mscm_value)less_than_v);
    mscm_runtime_push(rt, "<", (mscm_value)less_than_v);
    mscm_runtime_push(rt, "add", (mscm_value)add_v);
    mscm_runtime_push(rt, "+", (mscm_value)add_v);
    mscm_runtime_push(rt, "sub", (mscm_value)sub_v);
    mscm_runtime_push(rt, "-", (mscm_value)sub_v);
    mscm_runtime_push(rt, "mul", (mscm_value)mul_v);
    mscm_runtime_push(rt, "*", (mscm_value)mul_v);

    mscm_runtime_gc_add(rt, display_v);
    mscm_runtime_gc_add(rt, error_v);
    mscm_runtime_gc_add(rt, equals_v);
    mscm_runtime_gc_add(rt, less_than_v);
    mscm_runtime_gc_add(rt, add_v);
    mscm_runtime_gc_add(rt, sub_v);
    mscm_runtime_gc_add(rt, mul_v);
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

MSCM_NATIVE_FN(equals) {
    (void)scope;
    (void)ctx;

    if (narg != 2) {
        fprintf(stderr,
                "error: equals: expected 2 arguments, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    mscm_value a = args[0];
    mscm_value b = args[1];

    if (a == b) {
        return g_true_v;
    }

    if (a->type != b->type) {
        return 0;
    }

    switch (a->type) {
        case MSCM_TYPE_INT: {
            mscm_int *ia = (mscm_int*)a;
            mscm_int *ib = (mscm_int*)b;
            return ia->value == ib->value ? g_true_v : 0;
        }
        case MSCM_TYPE_FLOAT: {
            mscm_float *fa = (mscm_float*)a;
            mscm_float *fb = (mscm_float*)b;
            return fa->value == fb->value ? g_true_v : 0;
        }
        case MSCM_TYPE_STRING: case MSCM_TYPE_SYMBOL: {
            mscm_string *sa = (mscm_string*)a;
            mscm_string *sb = (mscm_string*)b;
            return !strcmp(sa->buf, sb->buf) ? g_true_v : 0;
        }
    }

    return 0;
}

MSCM_NATIVE_FN(less_than) {
    (void)scope;
    (void)ctx;

    if (narg != 2) {
        fprintf(stderr,
                "error: equals: expected 2 arguments, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    mscm_value a = args[0];
    mscm_value b = args[1];

    if (a == b) {
        return 0;
    }

    if (a->type != b->type) {
        fprintf(stderr,
                "error: less-than: "
                "expected 2 arguments of the same type\n");
        mscm_runtime_trace_exit(rt);
    }

    switch (a->type) {
        case MSCM_TYPE_INT: {
            mscm_int *ia = (mscm_int*)a;
            mscm_int *ib = (mscm_int*)b;
            return ia->value < ib->value ? g_true_v : 0;
        }
        case MSCM_TYPE_FLOAT: {
            mscm_float *fa = (mscm_float*)a;
            mscm_float *fb = (mscm_float*)b;
            return fa->value < fb->value ? g_true_v : 0;
        }
        default: {
            fprintf(stderr,
                    "error: less-than: "
                    "expected 2 arguments of numeric type\n");
            mscm_runtime_trace_exit(rt);
        }
    }

    return 0;
}

#define IMPL_NUMERIC_OP(OP) \
    (void)scope; \
    (void)ctx; \
    \
    if (narg == 0) { \
        return mscm_make_int(0); \
    } \
    \
    bool use_float = false; \
    int64_t acc = 0; \
    double facc = 0.0; \
    for (size_t i = 0; i < narg; i++) { \
        if (use_float || args[i]->type == MSCM_TYPE_FLOAT) { \
            use_float = true; \
            facc OP ((mscm_float*)args[i])->value; \
            break; \
        } \
        else if (args[i]->type != MSCM_TYPE_INT) { \
            fprintf(stderr, \
                    "error: builtin-add: %" PRIu64 \
                    "th arg: expected int or float value\n", \
                    (uint64_t)i); \
            mscm_runtime_trace_exit(rt); \
        } \
        else { \
            facc OP ((mscm_int*)args[i])->value; \
            acc OP ((mscm_int*)args[i])->value; \
        } \
    } \
    \
    mscm_value ret = use_float ? \
        mscm_make_float(facc) : \
        mscm_make_int(acc); \
    mscm_runtime_gc_add(rt, ret); \
    return ret;

MSCM_NATIVE_FN(add) { IMPL_NUMERIC_OP(+=); }
MSCM_NATIVE_FN(sub) { IMPL_NUMERIC_OP(-=); }
MSCM_NATIVE_FN(mul) { IMPL_NUMERIC_OP(*=); }
