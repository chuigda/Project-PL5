#include <inttypes.h>
#include <stddef.h>
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
MSCM_NATIVE_FN(mul);
MSCM_NATIVE_FN(string_concat);
MSCM_NATIVE_FN(make_pair);
MSCM_NATIVE_FN(car);
MSCM_NATIVE_FN(cdr);
MSCM_NATIVE_FN(list);

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
    mscm_value mul_v = mscm_make_native_function(mul, 0, 0);
    mscm_value strcat_v = mscm_make_native_function(string_concat, 0, 0);
    mscm_value cons_v = mscm_make_native_function(make_pair, 0, 0);
    mscm_value car_v = mscm_make_native_function(car, 0, 0);
    mscm_value cdr_v = mscm_make_native_function(cdr, 0, 0);
    mscm_value list_v = mscm_make_native_function(list, 0, 0);

    mscm_runtime_push(rt, "display", (mscm_value)display_v);
    mscm_runtime_push(rt, "error", (mscm_value)error_v);
    mscm_runtime_push(rt, "equals?", (mscm_value)equals_v);
    mscm_runtime_push(rt, "=", (mscm_value)equals_v);
    mscm_runtime_push(rt, "less-than?", (mscm_value)less_than_v);
    mscm_runtime_push(rt, "<", (mscm_value)less_than_v);
    mscm_runtime_push(rt, "add", (mscm_value)add_v);
    mscm_runtime_push(rt, "+", (mscm_value)add_v);
    mscm_runtime_push(rt, "mul", (mscm_value)mul_v);
    mscm_runtime_push(rt, "*", (mscm_value)mul_v);
    mscm_runtime_push(rt, "string-append", (mscm_value)strcat_v);
    mscm_runtime_push(rt, "string-concat", (mscm_value)strcat_v);
    mscm_runtime_push(rt, "~", (mscm_value)strcat_v);
    mscm_runtime_push(rt, "cons", (mscm_value)cons_v);
    mscm_runtime_push(rt, "car", (mscm_value)car_v);
    mscm_runtime_push(rt, "cdr", (mscm_value)cdr_v);
    mscm_runtime_push(rt, "list", (mscm_value)list_v);

    mscm_runtime_gc_add(rt, display_v);
    mscm_runtime_gc_add(rt, error_v);
    mscm_runtime_gc_add(rt, equals_v);
    mscm_runtime_gc_add(rt, less_than_v);
    mscm_runtime_gc_add(rt, add_v);
    mscm_runtime_gc_add(rt, mul_v);
    mscm_runtime_gc_add(rt, strcat_v);
    mscm_runtime_gc_add(rt, cons_v);
    mscm_runtime_gc_add(rt, car_v);
    mscm_runtime_gc_add(rt, cdr_v);
    mscm_runtime_gc_add(rt, list_v);
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

    if (a == 0 || b == 0) {
        return 0;
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

    if (a == 0 || b == 0) {
        fprintf(stderr,
                "error: less-than: "
                "expected 2 arguments of numeric type\n");
        mscm_runtime_trace_exit(rt);
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

#define IMPL_NUMERIC_OP(OP, OPNAME, INIT) \
    (void)scope; \
    (void)ctx; \
    \
    if (narg == 0) { \
        return mscm_make_int(0); \
    } \
    \
    bool use_float = false; \
    int64_t acc = INIT; \
    double facc = INIT; \
    for (size_t i = 0; i < narg; i++) { \
        if (use_float || args[i]->type == MSCM_TYPE_FLOAT) { \
            use_float = true; \
            facc OP ((mscm_float*)args[i])->value; \
            break; \
        } \
        else if (args[i]->type != MSCM_TYPE_INT) { \
            fprintf(stderr, \
                    "error: builtin-" OPNAME ": %" PRIu64 \
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

MSCM_NATIVE_FN(add) { IMPL_NUMERIC_OP(+=, "add", 0); }
MSCM_NATIVE_FN(mul) { IMPL_NUMERIC_OP(*=, "mul", 1); }

MSCM_NATIVE_FN(string_concat) {
    (void)scope;
    (void)ctx;

    if (narg == 0) {
        mscm_value ret = mscm_make_string(mscm_slice_from_cstr(""),
                                          false,
                                          0);
        mscm_runtime_gc_add(rt, ret);
        return ret;
    }

    size_t len = 0;
    for (size_t i = 0; i < narg; i++) {
        if (args[i]->type != MSCM_TYPE_STRING) {
            fprintf(stderr,
                    "error: string-concat: %" PRIu64
                    "th arg: expected string value\n",
                    (uint64_t)i);
            mscm_runtime_trace_exit(rt);
        }
        len += ((mscm_string*)args[i])->size;
    }

    mscm_value ret = mscm_alloc_string(len);
    mscm_string *sret = (mscm_string*)ret;
    memset(sret->buf, 0, len + 1);

    for (size_t i = 0; i < narg; i++) {
        mscm_string *s = (mscm_string*)args[i];
        strcat(sret->buf, s->buf);
    }

    mscm_runtime_gc_add(rt, ret);
    return ret;
}

MSCM_NATIVE_FN(make_pair) {
    (void)scope;
    (void)ctx;

    if (narg != 2) {
        fprintf(stderr,
                "error: cons: expected 2 arguments, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    mscm_value ret = mscm_make_pair(args[0], args[1]);
    mscm_runtime_gc_add(rt, ret);
    return ret;
}

MSCM_NATIVE_FN(car) {
    (void)scope;
    (void)ctx;

    if (narg != 1) {
        fprintf(stderr,
                "error: car: expected 1 argument, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    if (args[0]->type != MSCM_TYPE_PAIR) {
        fprintf(stderr,
                "error: car: expected pair value\n");
        mscm_runtime_trace_exit(rt);
    }

    mscm_pair *p = (mscm_pair*)args[0];
    return p->fst;
}

MSCM_NATIVE_FN(cdr) {
    (void)scope;
    (void)ctx;

    if (narg != 1) {
        fprintf(stderr,
                "error: car: expected 1 argument, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    if (args[0]->type != MSCM_TYPE_PAIR) {
        fprintf(stderr,
                "error: car: expected pair value\n");
        mscm_runtime_trace_exit(rt);
    }

    mscm_pair *p = (mscm_pair*)args[0];
    return p->snd;
}

MSCM_NATIVE_FN(list) {
    (void)scope;
    (void)ctx;

    mscm_value ret = 0;
    for (size_t i = narg; i > 0; i--) {
        ret = mscm_make_pair(args[i - 1], ret);
        mscm_runtime_gc_add(rt, ret);
    }
    return ret;
}
