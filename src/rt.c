#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rt.h"
#include "rt_impl.h"
#include "scope.h"
#include "syntax.h"
#include "util.h"
#include "value.h"
#include "scope_impl.h"

static mscm_value runtime_get(mscm_runtime *rt,
                              const char *name,
                              bool *ok);
static void runtime_push(mscm_runtime *rt,
                         const char *name,
                         mscm_value value);
static mscm_scope *runtime_push_scope(mscm_runtime *rt);
static void runtime_pop_scope(mscm_runtime *rt);
static void runtime_push_scope_chain(mscm_runtime *rt,
                                     mscm_scope *scope);
static void runtime_pop_scope_chain(mscm_runtime *rt);
static void runtime_add_rooted_group(mscm_runtime *rt,
                                     rooted_group *group);
static void runtime_remove_rooted_group(mscm_runtime *rt,
                                        rooted_group *group);
static mscm_scope *runtime_current_scope(mscm_runtime *rt);
static mscm_value runtime_apply(mscm_runtime *rt, mscm_apply *apply);
static bool node_is_ident(mscm_syntax_node node, char const *ident);
static void runtime_gc_collect(mscm_runtime *rt);
static void runtime_gc_mark(mscm_value value);
static void runtime_gc_mark_scope(mscm_scope *scope);
static char const* mscm_value_to_string(mscm_value value);

mscm_runtime *runtime_new() {
    MALLOC_CHK_RET(mscm_runtime, rt);

    rt->global_scope = mscm_scope_new(0);
    if (!rt->global_scope) {
        free(rt);
        return 0;
    }

    rt->scope_chain = malloc(sizeof(scope_chain_node));
    if (!rt->scope_chain) {
        free(rt);
        return 0;
    }

    rt->scope_chain->parent = 0;
    rt->scope_chain->chain = rt->global_scope;

    rt->gc_enabled = true;
    rt->alloc_count = 0;
    rt->scope_alloc_count = 0;
    rt->gc_pool = 0;
    rt->scope_pool = 0;
    rt->trace = 0;
    return rt;
}

void runtime_free(mscm_runtime *rt) {
    managed_value *iter = rt->gc_pool;
    while (iter) {
        managed_value *current = iter;
        iter = iter->next;
        mscm_free_value(current->value);
        free(current);
    }

    managed_scope *scope_iter = rt->scope_pool;
    while (scope_iter) {
        managed_scope *current = scope_iter;
        scope_iter = scope_iter->next;
        mscm_scope_free(current->scope);
        free(current);
    }

    scope_chain_node *scope_chain_iter = rt->scope_chain;
    while (iter) {
        scope_chain_node *current = scope_chain_iter;
        scope_chain_iter = scope_chain_iter->parent;
        free(current);
    }

    mscm_scope_free(rt->global_scope);
    free(rt);
}

void mscm_runtime_push(mscm_runtime *rt,
                       const char *name,
                       mscm_value value) {
    mscm_scope_push(rt->global_scope, name, value);
}

mscm_value runtime_eval(mscm_runtime *rt,
                        mscm_syntax_node node,
                        bool multiple) {
    mscm_value ret = 0;
    while (node) {
        switch (node->kind) {
            case MSCM_SYN_VALUE: {
                mscm_value_node *value_node = (mscm_value_node*)node;
                ret = value_node->value;
                break;
            }
            case MSCM_SYN_IDENT: {
                mscm_ident *ident = (mscm_ident*)node;
                bool ok;
                ret = runtime_get(rt, ident->ident, &ok);
                if (!ok) {
                    err_printf(node->file, node->line,
                            "undefined variable: %s", ident->ident);
                    mscm_runtime_trace_exit(rt);
                }
                break;
            }
            case MSCM_SYN_DEFVAR: {
                mscm_defvar *defvar = (mscm_defvar*)node;
                mscm_value value = runtime_eval(rt, defvar->init, true);
                runtime_push(rt, defvar->var_name, value);
                ret = 0;
                break;
            }
            case MSCM_SYN_BEGIN: {
                mscm_begin *begin = (mscm_begin*)node;
                ret = runtime_eval(rt, begin->content, true);
                break;
            }
            case MSCM_SYN_LAMBDA: case MSCM_SYN_DEFUN: {
                mscm_func_def *fndef = (mscm_func_def*)node;
                mscm_scope *capture_scope = runtime_current_scope(rt);
                ret = mscm_make_function(fndef, capture_scope);
                mscm_runtime_gc_add(rt, ret);
                if (node->kind == MSCM_SYN_DEFUN) {
                    bool defined;
                    mscm_scope_get(runtime_current_scope(rt),
                                   fndef->func_name,
                                   &defined);
                    if (defined) {
                        err_printf(node->file, node->line,
                                   "%s already defined in current scope",
                                   fndef->func_name);
                        mscm_runtime_trace_exit(rt);
                    }
                    runtime_push(rt, fndef->func_name, ret);
                }
                break;
            }
            case MSCM_SYN_COND: {
                mscm_cond *cond_node = (mscm_cond*)node;
                mscm_syntax_node cond = cond_node->cond_list;
                mscm_syntax_node then = cond_node->then_list;

                while (cond) {
                    if (node_is_ident(cond, "otherwise") ||
                        node_is_ident(cond, "else")) {
                        ret = runtime_eval(rt, then, false);
                        break;
                    }

                    mscm_value cond_result =
                        runtime_eval(rt, cond, false);
                    if (mscm_value_is_true(cond_result)) {
                        ret = runtime_eval(rt, then, false);
                        break;
                    }

                    cond = cond->next;
                    then = then->next;
                }

                ret = 0;
                break;
            }
            case MSCM_SYN_IF: {
                mscm_if *if_node = (mscm_if*)node;
                mscm_value cond_result =
                    runtime_eval(rt, if_node->cond, true);
                if (mscm_value_is_true(cond_result)) {
                    ret = runtime_eval(rt, if_node->then, true);
                }
                else if (if_node->otherwise) {
                    ret = runtime_eval(rt, if_node->otherwise, true);
                }
                break;
            }
            case MSCM_SYN_APPLY: {
                ret = runtime_apply(rt, (mscm_apply*)node);
                break;
            }
        }

        if (!multiple) {
            break;
        }

        node = node->next;
    }

    return ret;
}

void mscm_toggle_gc(mscm_runtime *rt, bool enable) {
    rt->gc_enabled = enable;
}

void mscm_runtime_gc_add(mscm_runtime *rt, mscm_value value) {
    if (rt->gc_enabled &&
        (rt->alloc_count >= GC_BUDGET ||
         rt->scope_alloc_count >= SCOPE_GC_BUDGET)) {
        runtime_gc_collect(rt);
    }

    managed_value *item = malloc(sizeof(managed_value));
    if (!item) {
        return;
    }

    item->value = value;
    item->next = rt->gc_pool;
    rt->gc_pool = item;
    rt->alloc_count += 1;
}

void mscm_runtime_trace_exit(mscm_runtime *rt) {
    stack_trace *trace = rt->trace;
    while (trace) {
        if (trace->fndef->kind == MSCM_SYN_DEFUN) {
            fprintf(stderr,
                    "\t when calling `%s` from %s:%" PRIu64 "\n",
                    trace->fndef->func_name,
                    trace->file ? trace->file : "unknown",
                    (uint64_t)trace->line);
        }
        else {
            fprintf(stderr,
                    "\t when calling <lambda %s:%" PRIu64
                    "> from %s:%" PRIu64 "\n",
                    trace->fndef->file, (uint64_t)trace->fndef->line,
                    trace->file ? trace->file : "unknown",
                    (uint64_t)trace->line);
        }

        trace = trace->next;
    }
    exit(1);
}

static mscm_value runtime_get(mscm_runtime *rt, const char *name, bool *ok) {
    return mscm_scope_get(rt->scope_chain->chain, name, ok);
}

static void runtime_push(mscm_runtime *rt,
                         const char *name,
                         mscm_value value) {
    mscm_scope_push(rt->scope_chain->chain, name, value);
}

static mscm_scope *runtime_current_scope(mscm_runtime *rt) {
    return rt->scope_chain->chain;
}

static mscm_value runtime_apply(mscm_runtime *rt, mscm_apply *apply) {
    mscm_value callee = runtime_eval(rt, apply->callee, true);
    if (!callee ||
        (callee->type != MSCM_TYPE_FUNCTION &&
         callee->type != MSCM_TYPE_NATIVE)) {
        err_printf(apply->callee->file, apply->callee->line,
                   "%s is not a function",
                   mscm_value_to_string(callee));
        mscm_runtime_trace_exit(rt);
    }

    rooted_group arg_roots = { 0, 0 };
    runtime_add_rooted_group(rt, &arg_roots);
    rooted_value *current_root = 0;
    mscm_syntax_node arg = apply->args;
    while (arg) {
        mscm_value arg_value = runtime_eval(rt, arg, false);
        rooted_value *arg_root = malloc(sizeof(rooted_value));
        if (!arg_root) {
            err_print(arg->file, arg->line, "out of memory");
            mscm_runtime_trace_exit(rt);
        }

        arg_root->value = arg_value;
        arg_root->next = 0;
        if (current_root) {
            current_root->next = arg_root;
            current_root = arg_root;
        }
        else {
            arg_roots.values = arg_root;
            current_root = arg_root;
        }

        arg = arg->next;
    }

    if (callee->type == MSCM_TYPE_FUNCTION) {
        runtime_remove_rooted_group(rt, &arg_roots);
        mscm_function *func = (mscm_function*)callee;
        mscm_func_def *fndef = func->fndef;

        if (func->scope) {
            runtime_push_scope_chain(rt, func->scope);
        }
        mscm_scope *func_scope = runtime_push_scope(rt);

        rooted_value *arg_iter = arg_roots.values;
        mscm_ident* param_iter = fndef->param_names;

        while (arg_iter && param_iter) {
            mscm_scope_push(func_scope,
                            param_iter->ident,
                            arg_iter->value);
            rooted_value *current = arg_iter;
            arg_iter = arg_iter->next;
            param_iter = (mscm_ident*)param_iter->next;
            free(current);
        }

        if (arg_iter || param_iter) {
            err_printf(apply->file, apply->line,
                       "when calling %s: argument count mismatch",
                       fndef->kind == MSCM_SYN_DEFUN
                           ? fndef->func_name : "lambda");
            mscm_runtime_trace_exit(rt);
        }

        runtime_push_scope(rt);

        stack_trace *trace = malloc(sizeof(stack_trace));
        if (trace) {
            trace->next = rt->trace;
            trace->file = apply->file;
            trace->line = apply->line;
            trace->fndef = fndef;
            rt->trace = trace;
        }
        mscm_value ret = runtime_eval(rt, fndef->body, true);
        if (trace) {
            rt->trace = trace->next;
            free(trace);
        }

        runtime_pop_scope(rt); /* function inner scope */
        runtime_pop_scope(rt); /* function scope */
        if (func->scope) {
            runtime_pop_scope_chain(rt);
        }

        return ret;
    }
    else {
        size_t narg = 0;
        rooted_value *arg_iter = arg_roots.values;
        while (arg_iter) {
            narg += 1;
            arg_iter = arg_iter->next;
        }

        mscm_value args[narg];
        arg_iter = arg_roots.values;
        for (size_t i = 0; i < narg; i++) {
            args[i] = arg_iter->value;
            rooted_value *current = arg_iter;
            arg_iter = arg_iter->next;
            free(current);
        }
        runtime_remove_rooted_group(rt, &arg_roots);

        mscm_native_function *native = (mscm_native_function*)callee;
        mscm_value ret = native->fnptr(rt,
                                       runtime_current_scope(rt),
                                       native->ctx,
                                       narg, args);
        return ret;
    }
}

static bool node_is_ident(mscm_syntax_node node, char const *ident) {
    return node->kind == MSCM_SYN_IDENT &&
           !strcmp(((mscm_ident*)node)->ident, ident);
}

static void runtime_gc_collect(mscm_runtime *rt) {
    managed_value *iter = rt->gc_pool;
    while (iter) {
        iter->value->gc_mark = false;
        iter = iter->next;
    }

    managed_scope *scope_iter = rt->scope_pool;
    while (scope_iter) {
        scope_iter->scope->gc_mark = false;
        scope_iter = scope_iter->next;
    }

    rooted_group *group_iter = rt->rooted_groups;
    while (group_iter) {
        rooted_value *value_iter = group_iter->values;
        while (value_iter) {
            runtime_gc_mark(value_iter->value);
            value_iter = value_iter->next;
        }
        group_iter = group_iter->next;
    }

    scope_chain_node *scope_chain_iter = rt->scope_chain;
    while (scope_chain_iter) {
        runtime_gc_mark_scope(scope_chain_iter->chain);
        scope_chain_iter = scope_chain_iter->parent;
    }

    iter = rt->gc_pool;
    while (iter && !iter[0].value->gc_mark) {
        managed_value *current = iter;
        iter = iter->next;
        mscm_free_value(current->value);
    }

    rt->gc_pool = iter;
    while (iter && iter->next) {
        if (!iter->next->value->gc_mark) {
            managed_value *freed = iter->next;
            iter->next = freed->next;
            mscm_free_value(freed->value);
        }
        else {
            iter = iter->next;
        }
    }

    scope_iter = rt->scope_pool;
    while (scope_iter && !scope_iter->scope->gc_mark) {
        managed_scope *current = scope_iter;
        scope_iter = scope_iter->next;
        mscm_scope_free(current->scope);
    }

    rt->scope_pool = scope_iter;
    while (scope_iter && scope_iter->next) {
        if (!scope_iter->next->scope->gc_mark) {
            managed_scope *freed = scope_iter->next;
            scope_iter->next = freed->next;
            mscm_scope_free(freed->scope);
        }
        else {
            scope_iter = scope_iter->next;
        }
    }

    rt->alloc_count = 0;
    rt->scope_alloc_count = 0;
}

static void runtime_gc_mark(mscm_value value) {
    if (!value) {
        return;
    }

    if (value->gc_mark) {
        return;
    }

    value->gc_mark = true;
    switch (value->type) {
        case MSCM_TYPE_PAIR: {
            mscm_pair *pair = (mscm_pair*)value;
            runtime_gc_mark(pair->fst);
            runtime_gc_mark(pair->snd);
            break;
        }
        case MSCM_TYPE_FUNCTION: {
            mscm_function *func = (mscm_function*)value;
            runtime_gc_mark_scope(func->scope);
            break;
        }
    }
}

static void runtime_gc_mark_scope(mscm_scope *scope) {
    while (scope) {
        if (scope->gc_mark) {
            scope = scope->parent;
            continue;
        }

        scope->gc_mark = true;
        for (size_t i = 0; i < BUCKET_COUNT; i++) {
            hash_item *chain = scope->buckets[i];
            while (chain) {
                runtime_gc_mark(chain->value);
                chain = chain->next;
            }
        }
        scope = scope->parent;
    }
}

static char const* mscm_value_to_string(mscm_value value) {
    switch (value->type) {
        case MSCM_TYPE_INT: return "<int>";
        case MSCM_TYPE_FLOAT: return "<float>";
        case MSCM_TYPE_STRING: return "<string>";
        case MSCM_TYPE_SYMBOL: return "<symbol>";
        case MSCM_TYPE_PAIR: return "<pair>";
        case MSCM_TYPE_FUNCTION: return "<function>";
        case MSCM_TYPE_HANDLE: return "<handle>";
        case MSCM_TYPE_NATIVE: return "<native>";
        default:
            assert(false);
            return "unknown";
    }
}

static mscm_scope *runtime_push_scope(mscm_runtime *rt) {
    mscm_scope *scope = mscm_scope_new(runtime_current_scope(rt));
    rt->scope_chain->chain = scope;

    managed_scope *item = malloc(sizeof(managed_scope));
    if (!item) {
        return scope;
    }

    item->scope = scope;
    item->next = rt->scope_pool;
    rt->scope_pool = item;
    rt->scope_alloc_count += 1;
    return scope;
}

static void runtime_pop_scope(mscm_runtime *rt) {
    mscm_scope *scope = rt->scope_chain->chain;
    rt->scope_chain->chain = scope->parent;
}

static void runtime_push_scope_chain(mscm_runtime *rt, mscm_scope *scope) {
    scope_chain_node *node = malloc(sizeof(scope_chain_node));
    if (!node) {
        err_print(0, 0, "out of memory");
        mscm_runtime_trace_exit(rt);
    }

    node->parent = rt->scope_chain;
    node->chain = scope;
    rt->scope_chain = node;
}

static void runtime_pop_scope_chain(mscm_runtime *rt) {
    scope_chain_node *node = rt->scope_chain;
    rt->scope_chain = node->parent;
    free(node);
}

static void runtime_add_rooted_group(mscm_runtime *rt, rooted_group *group) {
    group->next = rt->rooted_groups;
    rt->rooted_groups = group;
}

static void runtime_remove_rooted_group(mscm_runtime *rt,
                                        rooted_group *group) {
    if (rt->rooted_groups == group) {
        rt->rooted_groups = group->next;
        return;
    }
    else {
        rooted_group *iter = rt->rooted_groups;
        while (iter) {
            if (iter->next == group) {
                iter->next = group->next;
                return;
            }
            iter = iter->next;
        }
    }

    assert(false);
}
