#ifndef MINI_SCHEME_SCOPE_H
#define MINI_SCHEME_SCOPE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* mscm_value 的前向声明 */
typedef struct st_mscm_value_base *mscm_value;

/* 一个作用域 */
typedef struct st_mscm_scope mscm_scope;

/* 以 parent 为父作用域，创建一个新作用域 */
mscm_scope *mscm_scope_new(mscm_scope *parent);

/* 向作用域中推入一个值
 * 
 * 如果作用域中之前不存在 name，则将这个值加入作用域；
 * 否则更新作用域中 name 值。这个函数不会向父级作用域中查找。
 * */
void mscm_scope_push(mscm_scope *scope,
                     const char *name,
                     mscm_value value);

/* 更新作用域中的一个值
 *
 * 如果作用域中之前存在 name，则更新它的值，并将 *ok 设置为 true；
 * 否则什么都不做，并将 *ok 设置为 false。这个函数会向父级作用域查找。
 * */
void mscm_scope_set(mscm_scope *scope,
                    const char *name,
                    mscm_value value,
                    bool *ok);

/* 从作用域中获取一个值
 *
 * 如果作用域中存在 name，则返回它的值，并将 *ok 设置为 true；
 * 否则返回 NULL，并将 *ok 设置为 false。注意 NULL 也是合法的变量值，
 * 所以如果要确切知道作用域中是否存在 name，必须检查 *ok 的值。
 * 这个函数会向父级作用域中查找。
 * */
mscm_value mscm_scope_get(mscm_scope *scope, const char *name, bool *ok);

/* 从作用域中获取一个值，不向上查找
 *
 * 如果作用域中存在 name，则返回它的值，并将 *ok 设置为 true；
 * 否则返回 NULL，并将 *ok 设置为 false。注意 NULL 也是合法的变量值，
 * 所以如果要确切知道作用域中是否存贮器 name，必须检查 *ok 的值。
 * 这个函数不会向父级作用域中查找。
 * */
mscm_value mscm_scope_get_current(mscm_scope *scope,
                                  const char *name,
                                  bool *ok);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MINI_SCHEME_SCOPE_H */
