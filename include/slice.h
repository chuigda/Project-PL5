#ifndef MINI_SCHEME_SLICE_H
#define MINI_SCHEME_SLICE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 一个字符串切片 */
typedef struct {
    /* 切片的起始地址 */
    const char *start;
    /* 切片的长度 */
    size_t len;
} mscm_slice;

/* 从空终止 C 字符串创建字符串切片 */
mscm_slice mscm_slice_from_cstr(char const *cstr);

/* 空的字符串切片 */
#define MSCM_NULL_SLICE ((mscm_slice) { 0, 0 })

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MINI_SCHEME_SLICE_H */
