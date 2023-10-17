#ifndef MINI_SCHEME_RT_H
#define MINI_SCHEME_RT_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* mscm_value 的前向声明 */
typedef struct st_mscm_value_base *mscm_value;

/* mscm_scope 的前向声明 */
typedef struct st_mscm_scope mscm_scope;

/* 一个 Mini Scheme 运行时 */
typedef struct st_mscm_runtime mscm_runtime;

/* 将一个值置入运行时的全局作用域
 *
 * 如果 rt 的全局作用域中不存在 name，则将其加入全局作用域；
 * 如果 name 已经存在，则覆盖之前的值。
 * */
void mscm_runtime_push(mscm_runtime *rt,
                       const char *name,
                       mscm_value value);

/* 从运行时的全局作用域获取一个值
 *
 * 如果 rt 的全局作用域中存在 name，则返回它的值并将 *ok 设置为
 * true；否则返回 NULL 并将 *ok 设置为 false。注意 NULL 也是合法
 * 的变量值，所以若要确定全局作用域中是否存在 name，必须检查 *ok
 * 的值
 * */
mscm_value mscm_runtime_get(mscm_runtime *rt,
                            const char *name,
                            bool *ok);

/* 打印栈追踪并退出程序 */
void mscm_runtime_trace_exit(mscm_runtime *rt);

/* 启用或禁用垃圾回收 */
void mscm_gc_toggle(mscm_runtime *rt, bool enable);

/* 将一个值加入托管堆，可能触发垃圾回收 */
void mscm_gc_add(mscm_runtime *rt, mscm_value value);

/* 将一个值标记为可达 */
void mscm_gc_mark(mscm_value value);

/* 将一个作用域连同其中所有值标记为可达 */
void mscm_gc_mark_scope(mscm_scope *scope);

/* 分配一个类型 ID 供 handle 使用，参见 mscm_handle */
uint32_t mscm_runtime_alloc_type_id(mscm_runtime *rt);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MINI_SCHEME_RT_H */
