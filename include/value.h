#ifndef MINI_SCHEME_VALUE_H
#define MINI_SCHEME_VALUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "slice.h"

#ifdef __cplusplus
extern "C" {
#endif

/* mscm_scope 的前向声明 */
typedef struct st_mscm_scope mscm_scope;

/* mscm_func_def 的前向声明 */
typedef struct st_mscm_func_def mscm_func_def;

/* mscm_runtime 的前向声明 */
typedef struct st_mscm_runtime mscm_runtime;

/* MiniScheme 中的类型 */
enum {
    /* 整数类型 */
    MSCM_TYPE_INT      = 0,
    /* 浮点类型 */
    MSCM_TYPE_FLOAT    = 1,
    /* 字符串类型 */
    MSCM_TYPE_STRING   = 2,
    /* Symbol 类型 */
    MSCM_TYPE_SYMBOL   = 3,
    /* 序对 */
    MSCM_TYPE_PAIR     = 4,
    /* 函数类型 */
    MSCM_TYPE_FUNCTION = 5,
    /* 句柄类型 */
    MSCM_TYPE_HANDLE   = 6,
    /* native 函数类型 */
    MSCM_TYPE_NATIVE   = 7
};

/* 所有 Mini Scheme 对象类型共享的对象头部 */
#define MSCM_VALUE_COMMON \
    uint8_t type; \ /* 对象的类型 */
    bool gc_mark;   /* GC 标记 */

/* 所有 Mini Scheme 对象类型的“基类” */
typedef struct st_mscm_value_base {
    MSCM_VALUE_COMMON
} mscm_value_base, *mscm_value;

/* 整数类型 */
typedef struct {
    MSCM_VALUE_COMMON
    /* 整数值 */
    int64_t value;
} mscm_int;

/* 浮点类型 */
typedef struct {
    MSCM_VALUE_COMMON
    /* 浮点值 */
    double value;
} mscm_float;

/* 字符串类型 */
typedef struct {
    MSCM_VALUE_COMMON
    /* 字符串的长度 */
    size_t size;
    /* 字符串的内容（以 NUL 字符结尾） */
    char buf[];
} mscm_string;

/* 序对类型 */
typedef struct {
    MSCM_VALUE_COMMON
    /* 序对中的第一个值 */
    mscm_value fst;
    /* 序对中的第二个值 */
    mscm_value snd;
} mscm_pair;

typedef mscm_pair mscm_list;

/* 用于释放用户数据的“析构函数”指针类型 */
typedef void (*mscm_user_dtor)(void* ptr);
/* 用于标记用户数据中 mscm_value 的“标记函数”指针类型 */
typedef void (*mscm_user_marker)(void *ptr);

/* 句柄类型 */
typedef struct {
    MSCM_VALUE_COMMON
    /* 用户定义的类型 ID */
    uint32_t user_tid;
    /* 用户数据 */
    void *ptr;
    /* 用户数据析构函数 */
    mscm_user_dtor dtor;
    /* 用户数据标记函数 */
    mscm_user_marker marker;
} mscm_handle;

/* native 函数指针的类型 */
typedef mscm_value (*mscm_native_fnptr)(
    /* 运行时 */
    mscm_runtime *rt,
    /* 当前作用域 */
    mscm_scope *scope,
    /* 用户上下文 */
    void *ctx,
    /* 参数数量 */
    size_t narg,
    /* 参数 */
    mscm_value *args
);

/* native 函数类型 */
typedef struct {
    MSCM_VALUE_COMMON
    /* 函数名 */
    char const *name;
    /* 函数指针 */
    mscm_native_fnptr fnptr;
    /* 用户上下文 */
    void *ctx;
    /* 用户上下文析构函数 */
    mscm_user_dtor ctx_dtor;
    /* 用户上下文标记函数 */
    mscm_user_marker ctx_marker;
} mscm_native_function;

/* 获得一个类型 ID 对应的类型名 */
char const *mscm_type_name(uint8_t t);

/* 获取一个值的类型对应的类型名 */
char const *mscm_value_type_name(mscm_value value);

/* 创建一个整数对象 */
mscm_value mscm_make_int(int64_t value);

/* 创建一个浮点对象 */
mscm_value mscm_make_float(double value);

/* 创建一个字符串对象 
 *
 * escape 指定是否要对 value 进行转义，例如将 \n 替换为 LF，
 * 将 \\ 替换为 \。如果 escape 为 true 且 esc_ok 不为 NULL，
 * 转义是否成功将由 *esc_ok 指示。
 * */
mscm_value mscm_make_string(mscm_slice value, bool escape, bool *esc_ok);

/* 预分配一个字符串对象 */
mscm_value mscm_alloc_string(size_t size);

/* 创建一个序对 */
mscm_value mscm_make_pair(mscm_value fst, mscm_value snd);

/* 创建一个句柄 */
mscm_value mscm_make_handle(uint32_t user_tid,
                            void *ptr,
                            mscm_user_dtor dtor,
                            mscm_user_marker marker);

/* 创建一个 native 函数对象 */
mscm_value mscm_make_native_function(char const *name,
                                     mscm_native_fnptr fnptr,
                                     void *ctx,
                                     mscm_user_dtor ctx_dtor,
                                     mscm_user_marker ctx_marker);

/* 判断一个值是否为 true
 *
 * 在 Mini Scheme 中，null，整数 0，浮点数 0.0 和空字符串被视为 false，
 * 其他值都是 true。这个函数的返回结果和内建特殊形式 cond/if 的判定规则
 * 一致。
 * */
bool mscm_value_is_true(mscm_value value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MINI_SCHEME_VALUE_H */
