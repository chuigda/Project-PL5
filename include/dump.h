#ifndef MINI_SCHEME_DUMP_H
#define MINI_SCHEME_DUMP_H

#include "value.h"
#include "syntax.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 输出 syntax_node 的内容到 stdout */
void mscm_sytnax_dump(mscm_syntax_node syntax_node);

/* 以某种“合理”的方式打印 value 的值到 stdout */
void mscm_value_dump(mscm_value value);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MINI_SCHEME_DUMP_H */
