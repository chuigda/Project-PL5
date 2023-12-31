#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "rt.h"
#include "scope.h"
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
MSCM_NATIVE_FN(print);
MSCM_NATIVE_FN(println);
MSCM_NATIVE_FN(error);
MSCM_NATIVE_FN(equals);
MSCM_NATIVE_FN(less_than);
MSCM_NATIVE_FN(add);
MSCM_NATIVE_FN(mul);
MSCM_NATIVE_FN(sub);
MSCM_NATIVE_FN(div);
MSCM_NATIVE_FN(mod);
MSCM_NATIVE_FN(string_concat);
MSCM_NATIVE_FN(string_length);
MSCM_NATIVE_FN(string_ref);
MSCM_NATIVE_FN(string_set);
MSCM_NATIVE_FN(make_pair);
MSCM_NATIVE_FN(car);
MSCM_NATIVE_FN(cdr);
MSCM_NATIVE_FN(list);

MSCM_NATIVE_FN(set);
MSCM_NATIVE_FN(set_car);
MSCM_NATIVE_FN(set_cdr);

MSCM_NATIVE_FN(is_pair);
MSCM_NATIVE_FN(is_int);
MSCM_NATIVE_FN(is_float);
MSCM_NATIVE_FN(is_string);
MSCM_NATIVE_FN(is_symbol);

static mscm_value g_true_v;

void mscm_load_ext(mscm_runtime *rt) {
    g_true_v = mscm_make_string(mscm_slice_from_cstr("true"), false, 0);
    g_true_v->type = MSCM_TYPE_SYMBOL;
    mscm_runtime_push(rt, "true", (mscm_value)g_true_v);
    mscm_gc_add(rt, (mscm_value)g_true_v);
    mscm_runtime_push(rt, "false", 0);

    mscm_value display_v =
        mscm_make_native_function("display", display, 0, 0, 0);
    mscm_value print_v = mscm_make_native_function("print", print, 0, 0, 0);
    mscm_value println_v =
        mscm_make_native_function("println", println, 0, 0, 0);
    mscm_value error_v =
        mscm_make_native_function("error", error, 0, 0, 0);
    mscm_value equals_v =
        mscm_make_native_function("equals", equals, 0, 0, 0);
    mscm_value less_than_v =
        mscm_make_native_function("less-than", less_than, 0, 0, 0);
    mscm_value add_v = mscm_make_native_function("add", add, 0, 0, 0);
    mscm_value mul_v = mscm_make_native_function("mul", mul, 0, 0, 0);
    mscm_value sub_v = mscm_make_native_function("sub", sub, 0, 0, 0);
    mscm_value div_v = mscm_make_native_function("div", div, 0, 0, 0);
    mscm_value mod_v = mscm_make_native_function("mod", mod, 0, 0, 0);
    mscm_value strcat_v =
        mscm_make_native_function("strcat", string_concat, 0, 0, 0);
    mscm_value strlen_v =
        mscm_make_native_function("strlen", string_length, 0, 0, 0);
    mscm_value string_ref_v =
        mscm_make_native_function("string-ref", string_ref, 0, 0, 0);
    mscm_value string_set_v =
        mscm_make_native_function("string-set!", string_set, 0, 0, 0);
    mscm_value cons_v =
        mscm_make_native_function("make-pair", make_pair, 0, 0, 0);
    mscm_value car_v = mscm_make_native_function("car", car, 0, 0, 0);
    mscm_value cdr_v = mscm_make_native_function("cdr", cdr, 0, 0, 0);
    mscm_value list_v = mscm_make_native_function("list", list, 0, 0, 0);

    mscm_value set_v =
        mscm_make_native_function("set!", set, 0, 0, 0);
    mscm_value set_car_v =
        mscm_make_native_function("set-car!", set_car, 0, 0, 0);
    mscm_value set_cdr_v =
        mscm_make_native_function("set-cdr!", set_cdr, 0, 0, 0);

    mscm_value is_pair_v =
        mscm_make_native_function("pair?", is_pair, 0, 0, 0);
    mscm_value is_int_v =
        mscm_make_native_function("int?", is_int, 0, 0, 0);
    mscm_value is_float_v =
        mscm_make_native_function("float?", is_float, 0, 0, 0);
    mscm_value is_string_v =
        mscm_make_native_function("string?", is_string, 0, 0, 0);
    mscm_value is_symbol_v =
        mscm_make_native_function("symbol?", is_symbol, 0, 0, 0);

    mscm_runtime_push(rt, "display", (mscm_value)display_v);
    mscm_runtime_push(rt, "print", (mscm_value)print_v);
    mscm_runtime_push(rt, "println", (mscm_value)println_v);
    mscm_runtime_push(rt, "error", (mscm_value)error_v);
    mscm_runtime_push(rt, "equals?", (mscm_value)equals_v);
    mscm_runtime_push(rt, "=", (mscm_value)equals_v);
    mscm_runtime_push(rt, "less-than?", (mscm_value)less_than_v);
    mscm_runtime_push(rt, "<", (mscm_value)less_than_v);
    mscm_runtime_push(rt, "add", (mscm_value)add_v);
    mscm_runtime_push(rt, "+", (mscm_value)add_v);
    mscm_runtime_push(rt, "mul", (mscm_value)mul_v);
    mscm_runtime_push(rt, "*", (mscm_value)mul_v);
    mscm_runtime_push(rt, "sub", (mscm_value)sub_v);
    mscm_runtime_push(rt, "-", (mscm_value)sub_v);
    mscm_runtime_push(rt, "div", (mscm_value)div_v);
    mscm_runtime_push(rt, "/", (mscm_value)div_v);
    mscm_runtime_push(rt, "mod", (mscm_value)mod_v);
    mscm_runtime_push(rt, "%", (mscm_value)mod_v);
    mscm_runtime_push(rt, "string-append", (mscm_value)strcat_v);
    mscm_runtime_push(rt, "string-concat", (mscm_value)strcat_v);
    mscm_runtime_push(rt, "string-length", (mscm_value)strlen_v);
    mscm_runtime_push(rt, "string-ref", (mscm_value)string_ref_v);
    mscm_runtime_push(rt, "string-set!", (mscm_value)string_set_v);
    mscm_runtime_push(rt, "~", (mscm_value)strcat_v);
    mscm_runtime_push(rt, "cons", (mscm_value)cons_v);
    mscm_runtime_push(rt, "car", (mscm_value)car_v);
    mscm_runtime_push(rt, "cdr", (mscm_value)cdr_v);
    mscm_runtime_push(rt, "list", (mscm_value)list_v);

    mscm_runtime_push(rt, "set!", (mscm_value)set_v);
    mscm_runtime_push(rt, "set-car!", (mscm_value)set_car_v);
    mscm_runtime_push(rt, "set-cdr!", (mscm_value)set_cdr_v);
    
    mscm_runtime_push(rt, "pair?", (mscm_value)is_pair_v);
    mscm_runtime_push(rt, "int?", (mscm_value)is_int_v);
    mscm_runtime_push(rt, "float?", (mscm_value)is_float_v);
    mscm_runtime_push(rt, "string?", (mscm_value)is_string_v);
    mscm_runtime_push(rt, "symbol?", (mscm_value)is_symbol_v);

    mscm_gc_add(rt, display_v);
    mscm_gc_add(rt, print_v);
    mscm_gc_add(rt, println_v);
    mscm_gc_add(rt, error_v);
    mscm_gc_add(rt, equals_v);
    mscm_gc_add(rt, less_than_v);
    mscm_gc_add(rt, add_v);
    mscm_gc_add(rt, mul_v);
    mscm_gc_add(rt, sub_v);
    mscm_gc_add(rt, div_v);
    mscm_gc_add(rt, mod_v);
    mscm_gc_add(rt, strcat_v);
    mscm_gc_add(rt, strlen_v);
    mscm_gc_add(rt, string_ref_v);
    mscm_gc_add(rt, string_set_v);
    mscm_gc_add(rt, cons_v);
    mscm_gc_add(rt, car_v);
    mscm_gc_add(rt, cdr_v);
    mscm_gc_add(rt, list_v);

    mscm_gc_add(rt, set_v);
    mscm_gc_add(rt, set_car_v);
    mscm_gc_add(rt, set_cdr_v);

    mscm_gc_add(rt, is_pair_v);
    mscm_gc_add(rt, is_int_v);
    mscm_gc_add(rt, is_float_v);
    mscm_gc_add(rt, is_string_v);
    mscm_gc_add(rt, is_symbol_v);
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

MSCM_NATIVE_FN(print) {
    (void)rt;
    (void)scope;
    (void)ctx;

    for (size_t i = 0; i < narg; i++) {
        if (!args[i]) {
            printf("null");
            continue;
        }

        switch (args[i]->type) {
            case MSCM_TYPE_STRING: {
                mscm_string *s = (mscm_string*)args[i];
                printf("%s", s->buf);
                break;
            }
            default:
                mscm_value_dump(args[i]);
                break;
        }
    }
    return 0;
}

MSCM_NATIVE_FN(println) {
    print(rt, scope, ctx, narg, args);
    putchar('\n');
    return 0;
}

MSCM_NATIVE_FN(error) {
    (void)scope;
    (void)ctx;

    fprintf(stderr, "error: ");
    for (size_t i = 0; i < narg; i++) {
        switch (args[i]->type) {
        case MSCM_TYPE_STRING: 
        case MSCM_TYPE_SYMBOL: {
            mscm_string *s = (mscm_string*)args[i];
            fprintf(stderr, "%s", s->buf);
            break;
        }
        case MSCM_TYPE_INT: {
            mscm_int *s = (mscm_int*)args[i];
            fprintf(stderr, "%" PRId64, s->value);
            break;
        }
        case MSCM_TYPE_FLOAT: {
            mscm_float *s = (mscm_float*)args[i];
            fprintf(stderr, "%f", s->value);
            break;
        }
        case MSCM_TYPE_FUNCTION: {
            fprintf(stderr, "<function>");
            break;
        }
        case MSCM_TYPE_HANDLE: {
            fprintf(stderr, "<handle>");
            break;
        }
        case MSCM_TYPE_PAIR: {
            fprintf(stderr, "<pair>");
            break;
        }
        case MSCM_TYPE_NATIVE: {
            fprintf(stderr, "<native>");
            break;
        }
        }
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
                "expected 2 arguments of numeric type, got (%s, %s)\n",
                mscm_value_type_name(a),
                mscm_value_type_name(b));
        mscm_runtime_trace_exit(rt);
    }

    if (a->type != b->type) {
        fprintf(stderr,
                "error: less-than: "
                "expected 2 arguments of the same type, got (%s, %s)\n",
                mscm_value_type_name(a),
                mscm_value_type_name(b));
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
                    "expected 2 arguments of numeric type,"
                    " got (%s, %s)\n",
                    mscm_value_type_name(a),
                    mscm_value_type_name(b));
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
        mscm_value ret = mscm_make_int(0); \
        mscm_gc_add(rt, ret); \
        return ret; \
    } \
    \
    bool use_float = false; \
    int64_t acc = INIT; \
    double facc = INIT; \
    for (size_t i = 0; i < narg; i++) { \
        if (!args[i]) { \
            fprintf(stderr, \
                    "error: builtin-" OPNAME ": %" PRIu64 \
                    "th arg: expected int or float value, got %s\n", \
                    (uint64_t)i,\
                    mscm_value_type_name(args[i])); \
            mscm_runtime_trace_exit(rt); \
        } \
        if (use_float || args[i]->type == MSCM_TYPE_FLOAT) { \
            use_float = true; \
            facc OP ((mscm_float*)args[i])->value; \
            break; \
        } \
        else if (args[i]->type != MSCM_TYPE_INT) { \
            fprintf(stderr, \
                    "error: builtin-" OPNAME ": %" PRIu64 \
                    "th arg: expected int or float value, got %s\n", \
                    (uint64_t)i,\
                    mscm_value_type_name(args[i])); \
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
    mscm_gc_add(rt, ret); \
    return ret;

MSCM_NATIVE_FN(add) { IMPL_NUMERIC_OP(+=, "add", 0); }
MSCM_NATIVE_FN(mul) { IMPL_NUMERIC_OP(*=, "mul", 1); }

MSCM_NATIVE_FN(sub) {
    (void)scope;
    (void)ctx;

    if (narg != 2) {
        fprintf(stderr,
                "error: builtin-sub: expected 2 arguments, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    if (!args[0] || !args[1] ||
        (args[0]->type != MSCM_TYPE_INT &&
         args[0]->type != MSCM_TYPE_FLOAT) ||
         (args[1]->type != MSCM_TYPE_INT &&
          args[1]->type != MSCM_TYPE_FLOAT)) {
        fprintf(stderr,
                "error: builtin-sub: "
                "expected 2 arguments of numeric type, got (%s, %s)\n",
                mscm_value_type_name(args[0]),
                mscm_value_type_name(args[1]));
        mscm_runtime_trace_exit(rt);
    }

    uint8_t common_type = args[0]->type;
    if (common_type != args[1]->type) {
        common_type = MSCM_TYPE_FLOAT;
    }

    if (common_type == MSCM_TYPE_INT) {
        mscm_int *a = (mscm_int*)args[0];
        mscm_int *b = (mscm_int*)args[1];
        mscm_value ret = mscm_make_int(a->value - b->value);
        mscm_gc_add(rt, ret);
        return ret;
    }
    else {
        double a = args[0]->type == MSCM_TYPE_INT ?
            (double)((mscm_int*)args[0])->value :
            ((mscm_float*)args[0])->value;
        a -= args[1]->type == MSCM_TYPE_INT ?
            (double)((mscm_int*)args[1])->value :
            ((mscm_float*)args[1])->value;
        mscm_value ret = mscm_make_float(a);
        mscm_gc_add(rt, ret);
        return ret;
    }
}

MSCM_NATIVE_FN(div) {
    (void)scope;
    (void)ctx;

    if (narg != 2) {
        fprintf(stderr,
                "error: builtin-div: expected 2 arguments, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    if (!args[0] || !args[1] ||
        (args[0]->type != MSCM_TYPE_INT &&
         args[0]->type != MSCM_TYPE_FLOAT) ||
         (args[1]->type != MSCM_TYPE_INT &&
          args[1]->type != MSCM_TYPE_FLOAT)) {
        fprintf(stderr,
                "error: builtin-sub: "
                "expected 2 arguments of numeric type, got (%s, %s)\n",
                mscm_value_type_name(args[0]),
                mscm_value_type_name(args[1]));
        mscm_runtime_trace_exit(rt);
    }

    uint8_t common_type = args[0]->type;
    if (common_type != args[1]->type) {
        common_type = MSCM_TYPE_FLOAT;
    }

    if (common_type == MSCM_TYPE_INT) {
        mscm_int *a = (mscm_int*)args[0];
        mscm_int *b = (mscm_int*)args[1];
        if (b->value == 0) {
            fprintf(stderr,
                    "error: builtin-div: "
                    "division by zero\n");
            mscm_runtime_trace_exit(rt);
        }

        mscm_value ret = mscm_make_int(a->value / b->value);
        mscm_gc_add(rt, ret);
        return ret;
    }
    else {
        double a = args[0]->type == MSCM_TYPE_INT ?
            (double)((mscm_int*)args[0])->value :
            ((mscm_float*)args[0])->value;
        double b = args[1]->type == MSCM_TYPE_INT ?
            (double)((mscm_int*)args[1])->value :
            ((mscm_float*)args[1])->value;
        if (b == 0) {
            fprintf(stderr,
                    "error: builtin-div: "
                    "division by zero\n");
            mscm_runtime_trace_exit(rt);
        }

        mscm_value ret = mscm_make_float(a / b);
        mscm_gc_add(rt, ret);
        return ret;
    }
}

MSCM_NATIVE_FN(mod) {
    (void)scope;
    (void)ctx;

    if (narg != 2) {
        fprintf(stderr,
                "error: builtin-mod: expected 2 arguments, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    if (!args[0] || !args[1] ||
        args[0]->type != MSCM_TYPE_INT ||
        args[1]->type != MSCM_TYPE_INT) {
        fprintf(stderr,
                "error: builtin-mod: "
                "expected 2 arguments of int type, got (%s, %s)\n",
                mscm_value_type_name(args[0]),
                mscm_value_type_name(args[1]));
        mscm_runtime_trace_exit(rt);
    }

    mscm_int *a = (mscm_int*)args[0];
    mscm_int *b = (mscm_int*)args[1];
    if (b->value == 0) {
        fprintf(stderr,
                "error: builtin-mod: "
                "division by zero\n");
        mscm_runtime_trace_exit(rt);
    }

    mscm_value ret = mscm_make_int(a->value % b->value);
    mscm_gc_add(rt, ret);
    return ret;
}

MSCM_NATIVE_FN(string_concat) {
    (void)scope;
    (void)ctx;

    if (narg == 0) {
        mscm_value ret = mscm_make_string(mscm_slice_from_cstr(""),
                                          false,
                                          0);
        mscm_gc_add(rt, ret);
        return ret;
    }

    size_t len = 0;
    for (size_t i = 0; i < narg; i++) {
        if (!args[i] || args[i]->type != MSCM_TYPE_STRING) {
            fprintf(stderr,
                    "error: string-concat: %" PRIu64
                    "th arg: expected string value, got %s\n",
                    (uint64_t)i,
                    mscm_value_type_name(args[i]));
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

    mscm_gc_add(rt, ret);
    return ret;
}

MSCM_NATIVE_FN(string_length) {
    (void)scope;
    (void)ctx;

    if (narg != 1) {
        fprintf(stderr,
                "error: string-length: expected 1 argument, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    if (!args[0] || args[0]->type != MSCM_TYPE_STRING) {
        fprintf(stderr,
                "error: string-length: expected string value, got %s\n",
                mscm_value_type_name(args[0]));
        mscm_runtime_trace_exit(rt);
    }

    mscm_string *s = (mscm_string*)args[0];
    mscm_value ret = mscm_make_int(s->size);
    mscm_gc_add(rt, ret);
    return ret;
}

MSCM_NATIVE_FN(string_ref) {
    (void)scope;
    (void)ctx;

    if (narg != 2) {
        fprintf(stderr,
                "error: string-ref: expected 2 arguments, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    if (!args[0] || args[0]->type != MSCM_TYPE_STRING) {
        fprintf(stderr,
                "error: string-ref: expected string value, got %s\n",
                mscm_value_type_name(args[0]));
        mscm_runtime_trace_exit(rt);
    }

    if (!args[1] || args[1]->type != MSCM_TYPE_INT) {
        fprintf(stderr,
                "error: string-ref: expected int value, got %s\n",
                mscm_value_type_name(args[1]));
        mscm_runtime_trace_exit(rt);
    }

    mscm_string *s = (mscm_string*)args[0];
    mscm_int *i = (mscm_int*)args[1];
    if (i->value < 0 || i->value >= (int64_t)s->size) {
        fprintf(stderr,
                "error: string-ref: index out of bounds\n");
        mscm_runtime_trace_exit(rt);
    }

    int reti = s->buf[i->value];
    mscm_value ret = mscm_make_int(reti);
    mscm_gc_add(rt, ret);
    return ret;
}

MSCM_NATIVE_FN(string_set) {
    (void)scope;
    (void)ctx;

    if (narg != 3) {
        fprintf(stderr,
                "error: string-set!: expected 3 arguments, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    if (!args[0] || args[0]->type != MSCM_TYPE_STRING) {
        fprintf(stderr,
                "error: string-set!: expected string value, got %s\n",
                mscm_value_type_name(args[0]));
        mscm_runtime_trace_exit(rt);
    }

    if (!args[1] || args[1]->type != MSCM_TYPE_INT) {
        fprintf(stderr,
                "error: string-set!: expected int value, got %s\n",
                mscm_value_type_name(args[1]));
        mscm_runtime_trace_exit(rt);
    }

    if (!args[2] || args[2]->type != MSCM_TYPE_INT) {
        fprintf(stderr,
                "error: string-set!: expected int value, got %s\n",
                mscm_value_type_name(args[2]));
        mscm_runtime_trace_exit(rt);
    }

    mscm_string *s = (mscm_string*)args[0];
    mscm_int *i = (mscm_int*)args[1];
    mscm_int *c = (mscm_int*)args[2];
    if (i->value < 0 || i->value >= (int64_t)s->size) {
        fprintf(stderr,
                "error: string-set!: index out of bounds\n");
        mscm_runtime_trace_exit(rt);
    }

    s->buf[i->value] = (char)c->value;
    return 0;
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
    mscm_gc_add(rt, ret);
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

    if (!args[0] || args[0]->type != MSCM_TYPE_PAIR) {
        fprintf(stderr,
                "error: car: expected pair value, got %s\n",
                mscm_value_type_name(args[0]));
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

    if (!args[0] || args[0]->type != MSCM_TYPE_PAIR) {
        fprintf(stderr,
                "error: car: expected pair value, got %s\n",
                mscm_value_type_name(args[0]));
        mscm_runtime_trace_exit(rt);
    }

    mscm_pair *p = (mscm_pair*)args[0];
    return p->snd;
}

MSCM_NATIVE_FN(list) {
    (void)scope;
    (void)ctx;

    mscm_value ret = 0;
    mscm_gc_toggle(rt, false);
    for (size_t i = narg; i > 0; i--) {
        ret = mscm_make_pair(args[i - 1], ret);
        mscm_gc_add(rt, ret);
    }
    mscm_gc_toggle(rt, true);
    return ret;
}

MSCM_NATIVE_FN(set) {
    (void)ctx;

    if (narg != 2) {
        fprintf(stderr,
                "error: set!: expected 2 arguments, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    if (!args[0] ||
        (args[0]->type != MSCM_TYPE_SYMBOL &&
         args[0]->type != MSCM_TYPE_STRING)) {
        fprintf(stderr,
                "error: set!: expected symbol or string value,"
                " got %s\n",
                mscm_value_type_name(args[0]));
        mscm_runtime_trace_exit(rt);
    }

    mscm_string *s = (mscm_string*)args[0];
    bool ok;
    mscm_scope_set(scope, s->buf, args[1], &ok);
    if (!ok) {
        fprintf(stderr,
                "error: set!: symbol '%s not found\n",
                s->buf);
        mscm_runtime_trace_exit(rt);
    }

    return args[1];
}

MSCM_NATIVE_FN(set_car) {
    (void)scope;
    (void)ctx;

    if (narg != 2) {
        fprintf(stderr,
                "error: set-car!: expected 2 arguments, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    if (!args[0] || args[0]->type != MSCM_TYPE_PAIR) {
        fprintf(stderr,
                "error: set-car!: expected pair value, got %s\n",
                mscm_value_type_name(args[0]));
        mscm_runtime_trace_exit(rt);
    }

    mscm_pair *p = (mscm_pair*)args[0];
    p->fst = args[1];
    return 0;
}

MSCM_NATIVE_FN(set_cdr) {
    (void)scope;
    (void)ctx;

    if (narg != 2) {
        fprintf(stderr,
                "error: set-cdr!: expected 2 arguments, got %"
                PRIu64 "\n",
                narg);
        mscm_runtime_trace_exit(rt);
    }

    if (!args[0] || args[0]->type != MSCM_TYPE_PAIR) {
        fprintf(stderr,
                "error: set-cdr!: expected pair value, got %s\n",
                mscm_value_type_name(args[0]));
        mscm_runtime_trace_exit(rt);
    }

    mscm_pair *p = (mscm_pair*)args[0];
    p->snd = args[1];
    return 0;
}

#define MSCM_MAKE_ISA_FN(FNAME, TYPE) \
    MSCM_NATIVE_FN(FNAME) { \
        (void)scope; \
        (void)ctx; \
        \
        if (narg != 1) { \
            fprintf(stderr, \
                    "error: " #FNAME ": expected 1 argument, got %" \
                    PRIu64 "\n", \
                    narg); \
            mscm_runtime_trace_exit(rt); \
        } \
        \
        if (!args[0] || args[0]->type != TYPE) { \
            return 0; \
        } \
        else { \
            return g_true_v; \
        } \
    }

MSCM_MAKE_ISA_FN(is_pair, MSCM_TYPE_PAIR);
MSCM_MAKE_ISA_FN(is_int, MSCM_TYPE_INT);
MSCM_MAKE_ISA_FN(is_float, MSCM_TYPE_FLOAT);
MSCM_MAKE_ISA_FN(is_string, MSCM_TYPE_STRING);
MSCM_MAKE_ISA_FN(is_symbol, MSCM_TYPE_SYMBOL);
