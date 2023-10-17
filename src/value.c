#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "scope.h"
#include "value_impl.h"
#include "util.h"

char const *mscm_type_name(uint8_t t) {
    switch (t) {
        case MSCM_TYPE_INT:
            return "int";
        case MSCM_TYPE_FLOAT:
            return "float";
        case MSCM_TYPE_STRING:
            return "string";
        case MSCM_TYPE_PAIR:
            return "pair";
        case MSCM_TYPE_FUNCTION:
            return "function";
        case MSCM_TYPE_HANDLE:
            return "handle";
        case MSCM_TYPE_NATIVE:
            return "native";
        default:
            return "unknown";
    }
}

char const *mscm_value_type_name(mscm_value value) {
    if (!value) {
        return "null";
    }

    return mscm_type_name(value->type);
}

#define MSCM_VALUE_COMMON_INIT(VALUE, TYPE) \
    VALUE->type = TYPE; \
    VALUE->gc_mark = false;

mscm_value mscm_make_int(int64_t value) {
    MALLOC_CHK_RET(mscm_int, ret);
    MSCM_VALUE_COMMON_INIT(ret, MSCM_TYPE_INT);
    ret->value = value;
    return (mscm_value)ret;
}

mscm_value mscm_make_float(double value) {
    MALLOC_CHK_RET(mscm_float, ret);
    MSCM_VALUE_COMMON_INIT(ret, MSCM_TYPE_FLOAT);
    ret->value = value;
    return (mscm_value)ret;
}

mscm_value mscm_make_string(mscm_slice value,
                            bool escape,
                            bool *esc_ok) {
    mscm_string *ret = malloc(sizeof(mscm_string) + value.len + 1);
    if (!ret) {
        return 0;
    }

    MSCM_VALUE_COMMON_INIT(ret, MSCM_TYPE_STRING);

    if (!escape) {
        ret->size = value.len;
        strncpy(ret->buf, value.start, value.len);
        ret->buf[value.len] = '\0';
    }
    else {
        size_t i_in = 0, i_out = 0;
        while (i_in < value.len) {
            if (value.start[i_in] == '\\') {
                i_in += 1;
                switch (value.start[i_in]) {
                    case 'n':
                        ret->buf[i_out] = '\n';
                        break;
                    case 't':
                        ret->buf[i_out] = '\t';
                        break;
                    case 'r':
                        ret->buf[i_out] = '\r';
                        break;
                    case 'f':
                        ret->buf[i_out] = '\f';
                        break;
                    case 'v':
                        ret->buf[i_out] = '\v';
                        break;
                    case '\\':
                        ret->buf[i_out] = '\\';
                        break;
                    case '"':
                        ret->buf[i_out] = '"';
                        break;
                    default:
                        free(ret);
                        if (esc_ok) {
                            *esc_ok = false;
                        }
                        return 0;
                }
            }
            else {
                ret->buf[i_out] = value.start[i_in];
            }
            i_in += 1;
            i_out += 1;
        }

        ret->size = i_out;
        ret->buf[i_out] = '\0';
        if (esc_ok) {
            *esc_ok = true;
        }
    }
    return (mscm_value)ret;
}

mscm_value mscm_alloc_string(size_t size) {
    mscm_string *ret = malloc(sizeof(mscm_string) + size + 1);
    if (!ret) {
        return 0;
    }

    MSCM_VALUE_COMMON_INIT(ret, MSCM_TYPE_STRING);
    ret->size = size;
    ret->buf[size] = '\0';
    return (mscm_value)ret;
}

mscm_value mscm_make_pair(mscm_value fst, mscm_value snd) {
    MALLOC_CHK_RET(mscm_pair, ret);
    MSCM_VALUE_COMMON_INIT(ret, MSCM_TYPE_PAIR);
    ret->fst = fst;
    ret->snd = snd;
    return (mscm_value)ret;
}

mscm_value mscm_make_function(mscm_func_def *fndef, mscm_scope *scope) {
    MALLOC_CHK_RET(mscm_function, ret);
    MSCM_VALUE_COMMON_INIT(ret, MSCM_TYPE_FUNCTION);
    ret->fndef = fndef;
    ret->scope = scope;
    return (mscm_value)ret;
}

mscm_value mscm_make_handle(uint32_t user_tid,
                            void *ptr,
                            mscm_user_dtor dtor,
                            mscm_user_marker marker) {
    MALLOC_CHK_RET(mscm_handle, ret);
    MSCM_VALUE_COMMON_INIT(ret, MSCM_TYPE_HANDLE);
    ret->user_tid = user_tid;
    ret->ptr = ptr;
    ret->dtor = dtor;
    ret->marker = marker;
    return (mscm_value)ret;
}

mscm_value mscm_make_native_function(char const *name,
                                     mscm_native_fnptr fnptr,
                                     void *ctx,
                                     mscm_user_dtor ctx_dtor,
                                     mscm_user_marker ctx_marker) {
    MALLOC_CHK_RET(mscm_native_function, ret);
    MSCM_VALUE_COMMON_INIT(ret, MSCM_TYPE_NATIVE);
    ret->name = name;
    ret->fnptr = fnptr;
    ret->ctx = ctx;
    ret->ctx_dtor = ctx_dtor;
    ret->ctx_marker = ctx_marker;
    return (mscm_value)ret;
}

void mscm_free_value(mscm_value value) {
    if (!value) {
        return;
    }

    switch (value->type) {
        case MSCM_TYPE_HANDLE: {
            mscm_handle *handle = (mscm_handle*)value;
            if (handle->dtor) {
                handle->dtor(handle->ptr);
            }
            break;
        }
        case MSCM_TYPE_NATIVE: {
            mscm_native_function *fn = (mscm_native_function*)value;
            if (fn->ctx_dtor) {
                fn->ctx_dtor(fn->ctx);
            }
            break;
        }
    }

    free(value);
}

void mscm_free_value_deep(mscm_value value) {
    if (!value) {
        return;
    }

    switch (value->type) {
        case MSCM_TYPE_HANDLE: {
            mscm_handle *handle = (mscm_handle*)value;
            if (handle->dtor) {
                handle->dtor(handle->ptr);
            }
            break;
        }
        case MSCM_TYPE_NATIVE: {
            mscm_native_function *fn = (mscm_native_function*)value;
            if (fn->ctx_dtor) {
                fn->ctx_dtor(fn->ctx);
            }
            break;
        }
        case MSCM_TYPE_PAIR: {
            mscm_pair *pair = (mscm_pair*)value;
            mscm_free_value_deep(pair->fst);
            mscm_free_value_deep(pair->snd);
            break;
        }
    }

    free(value);
}

bool mscm_value_is_true(mscm_value value) {
    if (!value) {
        return false;
    }

    switch (value->type) {
        case MSCM_TYPE_INT: {
            mscm_int *i = (mscm_int*)value;
            return i->value != 0;
        }
        case MSCM_TYPE_FLOAT: {
            mscm_float *f = (mscm_float*)value;
            return f->value != 0.0;
        }
        case MSCM_TYPE_STRING: {
            mscm_string *s = (mscm_string*)value;
            return s->size != 0;
        }
        default:
            return true;
    }
}
