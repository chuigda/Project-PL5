#ifndef MINI_SCHEME_VALUE_H
#define MINI_SCHEME_VALUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "slice.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct st_mscm_scope mscm_scope;
typedef struct st_mscm_func_def mscm_func_def;
typedef struct st_mscm_runtime mscm_runtime;

enum {
    MSCM_TYPE_INT      = 0,
    MSCM_TYPE_FLOAT    = 1,
    MSCM_TYPE_STRING   = 2,
    MSCM_TYPE_SYMBOL   = 3,
    MSCM_TYPE_PAIR     = 4,
    MSCM_TYPE_FUNCTION = 5,
    MSCM_TYPE_HANDLE   = 6,
    MSCM_TYPE_NATIVE   = 7
};

#define MSCM_VALUE_COMMON \
    uint8_t type; \
    bool gc_mark;

typedef struct st_mscm_value_base {
    MSCM_VALUE_COMMON
} mscm_value_base, *mscm_value;

typedef struct {
    MSCM_VALUE_COMMON
    int64_t value;
} mscm_int;

typedef struct {
    MSCM_VALUE_COMMON
    double value;
} mscm_float;

typedef struct {
    MSCM_VALUE_COMMON
    size_t size;
    char buf[];
} mscm_string;

typedef struct {
    MSCM_VALUE_COMMON
    mscm_value fst, snd;
} mscm_pair;

typedef mscm_pair mscm_list;

typedef void (*mscm_user_dtor)(void* ptr);
typedef void (*mscm_user_marker)(void *ptr);

typedef struct {
    MSCM_VALUE_COMMON
    uint32_t user_tid;
    void *ptr;
    mscm_user_dtor dtor;
    mscm_user_marker marker;
} mscm_handle;

typedef mscm_value (*mscm_native_fnptr)(
    mscm_runtime *rt,
    mscm_scope *scope,
    void *ctx,
    size_t narg,
    mscm_value *args
);

typedef struct {
    MSCM_VALUE_COMMON
    mscm_native_fnptr fnptr;
    void *ctx;
    mscm_user_dtor ctx_dtor;
    mscm_user_marker ctx_marker;
} mscm_native_function;

char const *mscm_type_name(uint8_t t);
char const *mscm_value_type_name(mscm_value value);

mscm_value mscm_make_int(int64_t value);
mscm_value mscm_make_float(double value);
mscm_value mscm_make_string(mscm_slice value, bool escape, bool *esc_ok);
mscm_value mscm_alloc_string(size_t size);
mscm_value mscm_make_pair(mscm_value fst, mscm_value snd);
mscm_value mscm_make_handle(uint32_t user_tid,
                            void *ptr,
                            mscm_user_dtor dtor,
                            mscm_user_marker marker);
mscm_value mscm_make_native_function(mscm_native_fnptr fnptr,
                                     void *ctx,
                                     mscm_user_dtor ctx_dtor,
                                     mscm_user_marker ctx_marker);

bool mscm_value_is_true(mscm_value value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MINI_SCHEME_VALUE_H */
