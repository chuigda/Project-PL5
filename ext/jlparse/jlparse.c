#include <julia.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>

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

MSCM_NATIVE_FN(parse);
MSCM_NATIVE_FN(parse_str);

static mscm_value g_expr_v;
static mscm_value g_linenum_node_v;
static mscm_value g_true_v;

void mscm_load_ext(mscm_runtime *rt) {
    jl_init();

    mscm_value parse_v =
        mscm_make_native_function("jl-parse", parse, 0, 0, 0);
    mscm_value parse_str_v =
        mscm_make_native_function("jl-parse-string", parse_str, 0, 0, 0);

    mscm_runtime_push(rt, "jl-parse", parse_v);
    mscm_runtime_push(rt, "jl-parse-string", parse_str_v);
    mscm_gc_add(rt, parse_v);
    mscm_gc_add(rt, parse_str_v);

    g_expr_v =
        mscm_make_string(mscm_slice_from_cstr("expr"), 0, 0);
    g_linenum_node_v =
        mscm_make_string(mscm_slice_from_cstr("linenum"), 0, 0);
    g_expr_v->type = MSCM_TYPE_SYMBOL;
    g_linenum_node_v->type = MSCM_TYPE_SYMBOL;
    mscm_runtime_push(rt, "expr", g_expr_v);
    mscm_runtime_push(rt, "linenum", g_linenum_node_v);
    mscm_gc_add(rt, g_expr_v);
    mscm_gc_add(rt, g_linenum_node_v);

    bool ok;
    g_true_v = mscm_runtime_get(rt, "true", &ok);
    assert(ok && g_true_v);
}

static char* read_to_string(char const* filename);
static mscm_value jl2mscm(mscm_runtime *rt, jl_value_t *jval);
static mscm_value imp_jl2mscm(mscm_runtime *rt, jl_value_t *jval);

MSCM_NATIVE_FN(parse) {
    (void)scope;
    (void)ctx;

    if (narg != 1) {
        fprintf(stderr,
                "jl-parse: expected 1 argument, got %" PRIu64 "\n",
                (uint64_t)narg);
        mscm_runtime_trace_exit(rt);
    }

    mscm_value arg = args[0];
    if (!arg || arg->type != MSCM_TYPE_STRING) {
        fprintf(stderr, "jl-parse: expected string argument\n");
        mscm_runtime_trace_exit(rt);
    }

    mscm_string* filename = (mscm_string*)arg;
    char *str = read_to_string(filename->buf);
    if (!str) {
        fprintf(stderr, "jl-parse: could not read file %s\n",
                filename->buf);
        mscm_runtime_trace_exit(rt);
    }
    
    jl_value_t *jexpr = jl_parse_all(str, strlen(str),
                                    filename->buf, filename->size, 1);
    if (!jexpr) {
        fprintf(stderr, "jl-parse: could not parse file %s\n",
                filename->buf);
        mscm_runtime_trace_exit(rt);
    }

    mscm_gc_toggle(rt, false);
    mscm_value ret = jl2mscm(rt, jexpr);
    mscm_gc_toggle(rt, true);
    return ret;
}

MSCM_NATIVE_FN(parse_str) {
    (void)scope;
    (void)ctx;

    if (narg != 1 && narg != 2) {
        fprintf(stderr,
                "jl-parse-string: expected 1 or 2 arguments,"
                " got %" PRIu64 "\n",
                (uint64_t)narg);
        mscm_runtime_trace_exit(rt);
    }

    mscm_value arg0 = args[0];
    if (!arg0 || arg0->type != MSCM_TYPE_STRING) {
        fprintf(stderr, "jl-parse-string: expected string argument\n");
        mscm_runtime_trace_exit(rt);
    }
    mscm_string *src = (mscm_string*)arg0;
    char const* srcbuf = src->buf;

    char const* filename = "drill";
    if (narg == 2) {
        mscm_value arg1 = args[1];
        if (arg1) {
            if (arg1->type != MSCM_TYPE_STRING) {
                fprintf(stderr,
                        "jl-parse-string: expected string argument\n");
                mscm_runtime_trace_exit(rt);
            }

            mscm_string *filename_str = (mscm_string*)arg1;
            filename = filename_str->buf;
        }
    }

    jl_value_t *jexpr = jl_parse_all(srcbuf, src->size,
                                     filename, strlen(filename), 1);
    if (!jexpr) {
        fprintf(stderr, "jl-parse-string: could not parse string\n");
        mscm_runtime_trace_exit(rt);
    }

    mscm_gc_toggle(rt, false);
    mscm_value ret = jl2mscm(rt, jexpr);
    mscm_gc_toggle(rt, true);
    return ret;
}

static char* read_to_string(char const *file) {
    FILE *f = fopen(file, "rb");
    if (!f) {
        return 0;
    }

    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(len + 1);
    if (!buf) {
        fclose(f);
        return 0;
    }

    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

static mscm_value jl2mscm(mscm_runtime *rt, jl_value_t *jval) {
    mscm_value ret = imp_jl2mscm(rt, jval);
    mscm_gc_add(rt, ret);
    return ret;
}

static mscm_value imp_jl2mscm(mscm_runtime *rt, jl_value_t *jval) {
    if (jl_is_nothing(jval)) {
        return 0;
    }

    if (jl_is_symbol(jval)) {
        char const* name = jl_symbol_name((jl_sym_t*)jval);
        mscm_slice name_slice = mscm_slice_from_cstr(name);
        mscm_value ret = mscm_make_string(name_slice, false, 0);
        ret->type = MSCM_TYPE_SYMBOL;
        return (mscm_value)ret;
    }
    else if (jl_is_string(jval)) {
        char const *str = jl_string_data(jval);
        mscm_slice str_slice = mscm_slice_from_cstr(str);
        return (mscm_value)mscm_make_string(str_slice, false, 0);
    }
    else if (jl_is_int64(jval)) {
        int64_t unboxed = jl_unbox_int64(jval);
        return (mscm_value)mscm_make_int(unboxed);
    }
    else if (jl_is_int32(jval)) {
        int32_t unboxed = jl_unbox_int32(jval);
        return (mscm_value)mscm_make_int(unboxed);
    }
    else if (jl_is_int16(jval)) {
        int16_t unboxed = jl_unbox_int16(jval);
        return (mscm_value)mscm_make_int(unboxed);
    }
    else if (jl_is_int8(jval)) {
        int8_t unboxed = jl_unbox_int8(jval);
        return (mscm_value)mscm_make_int(unboxed);
    }
    else if (jl_is_bool(jval)) {
        bool unboxed = jl_unbox_bool(jval);
        if (unboxed) {
            return g_true_v;
        }
        else {
            return 0;
        }
    }
    else if (jl_typeis(jval, jl_float64_type)) {
        double unboxed = jl_unbox_float64(jval);
        return (mscm_value)mscm_make_float(unboxed);
    }
    else if (jl_typeis(jval, jl_float32_type)) {
        float unboxed = jl_unbox_float32(jval);
        return (mscm_value)mscm_make_float(unboxed);
    }
    else if (jl_typeis(jval, jl_expr_type)) {
        jl_value_t *head = jl_get_field(jval, "head");
        jl_value_t *args = jl_get_field(jval, "args");

        mscm_value mhead = jl2mscm(rt, head);
        mscm_value margs = jl2mscm(rt, args);
        mscm_value data_pair = mscm_make_pair(mhead, margs);
        mscm_gc_add(rt, data_pair);
        return mscm_make_pair(g_expr_v, data_pair);
    }
    else if (jl_typeis(jval, jl_linenumbernode_type)) {
        jl_value_t *file = jl_get_field(jval, "file");
        jl_value_t *line = jl_get_field(jval, "line");
        mscm_value mfile = jl2mscm(rt, file);
        mscm_value mline = jl2mscm(rt, line);
        mscm_value data_pair =  mscm_make_pair(mfile, mline);
        mscm_gc_add(rt, data_pair);
        return mscm_make_pair(g_linenum_node_v, data_pair);
    }
    else if (jl_is_array(jval)) {
        mscm_value ret = 0;
        mscm_value tail = 0;

        size_t length = jl_array_len(jval);
        for (size_t i = 0; i < length; i++) {
            jl_value_t *elem = jl_arrayref((jl_array_t*)jval, i);
            mscm_value mval = jl2mscm(rt, elem);
            if (!ret) {
                tail = mscm_make_pair(mval, 0);
                ret = tail;
            }
            else {
                mscm_value newtail = mscm_make_pair(mval, 0);
                ((mscm_pair*)tail)->snd = newtail;
                tail = newtail;
                mscm_gc_add(rt, tail);
            }
        }

        return ret;
    }
    else {
        mscm_gc_toggle(rt, true);
        fprintf(stderr, "jl2mscm: unknown type %s\n",
                jl_typename_str(jval));
        mscm_runtime_trace_exit(rt);
        /* unreachable, just make GCC happy */
        return 0;
    }
}
