#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "syntax.h"
#include "value.h"
#include "util.h"

#define MSCM_SYNTAX_NODE_COMMON_INIT(NODE, KIND, FILE, LINE) \
    NODE->kind = KIND; \
    NODE->file = FILE; \
    NODE->line = LINE; \
    NODE->next = 0;

mscm_syntax_node mscm_make_value_node(char const *file,
                                      size_t line,
                                      mscm_value value) {
    MALLOC_CHK_RET(mscm_value_node, ret);
    MSCM_SYNTAX_NODE_COMMON_INIT(ret, MSCM_SYN_VALUE, file, line);
    ret->value = value;
    return (mscm_syntax_node)ret;
}

mscm_syntax_node mscm_make_ident(char const *file,
                                 size_t line,
                                 mscm_slice ident) {
    mscm_ident *ret = malloc(sizeof(mscm_ident) + ident.len + 1);
    if (!ret) {
        return 0;
    }

    MSCM_SYNTAX_NODE_COMMON_INIT(ret, MSCM_SYN_IDENT, file, line);
    strncpy(ret->ident, ident.start, ident.len);
    ret->ident[ident.len] = '\0';
    return (mscm_syntax_node)ret;
}

mscm_syntax_node mscm_make_apply(char const *file,
                                 size_t line,
                                 mscm_syntax_node callee,
                                 mscm_syntax_node args) {
    MALLOC_CHK_RET(mscm_apply, ret);
    MSCM_SYNTAX_NODE_COMMON_INIT(ret, MSCM_SYN_APPLY, file, line);
    ret->callee = callee;
    ret->args = args;
    return (mscm_syntax_node)ret;
}

mscm_syntax_node mscm_make_begin(char const *file,
                                 size_t line,
                                 mscm_syntax_node content) {
    MALLOC_CHK_RET(mscm_begin, ret);
    MSCM_SYNTAX_NODE_COMMON_INIT(ret, MSCM_SYN_BEGIN, file, line);
    ret->content = content;
    return (mscm_syntax_node)ret;
}

mscm_syntax_node mscm_make_lambda(char const *file,
                                  size_t line,
                                  mscm_ident *param_names,
                                  mscm_syntax_node body) {
    MALLOC_CHK_RET(mscm_func_def, ret);
    MSCM_SYNTAX_NODE_COMMON_INIT(ret, MSCM_SYN_LAMBDA, file, line);
    ret->param_names = param_names;
    ret->body = body;
    return (mscm_syntax_node)ret;
}

mscm_syntax_node mscm_make_cond(char const *file,
                                size_t line,
                                mscm_syntax_node cond_list,
                                mscm_syntax_node then_list) {
    MALLOC_CHK_RET(mscm_cond, ret);
    MSCM_SYNTAX_NODE_COMMON_INIT(ret, MSCM_SYN_COND, file, line);
    ret->cond_list = cond_list;
    ret->then_list = then_list;
    return (mscm_syntax_node)ret;
}

mscm_syntax_node mscm_make_if(char const *file,
                              size_t line,
                              mscm_syntax_node cond,
                              mscm_syntax_node then,
                              mscm_syntax_node otherwise) {
    MALLOC_CHK_RET(mscm_if, ret);
    MSCM_SYNTAX_NODE_COMMON_INIT(ret, MSCM_SYN_IF, file, line);
    ret->cond = cond;
    ret->then = then;
    ret->otherwise = otherwise;
    return (mscm_syntax_node)ret;
}

mscm_syntax_node mscm_make_defvar(char const *file,
                                  size_t line,
                                  mscm_slice var_name,
                                  mscm_syntax_node init) {
    mscm_defvar *ret = malloc(sizeof(mscm_defvar) + var_name.len + 1);
    if (!ret) {
        return 0;
    }

    MSCM_SYNTAX_NODE_COMMON_INIT(ret, MSCM_SYN_DEFVAR, file, line);
    ret->init = init;
    strncpy(ret->var_name, var_name.start, var_name.len);
    ret->var_name[var_name.len] = 0;
    return (mscm_syntax_node)ret;
}

mscm_syntax_node mscm_make_func_def(char const *file,
                                    size_t line,
                                    mscm_slice func_name,
                                    mscm_ident *param_names,
                                    mscm_syntax_node body) {
    mscm_func_def *ret =
        malloc(sizeof(mscm_func_def) + func_name.len + 1);
    if (!ret) {
        return 0;
    }

    MSCM_SYNTAX_NODE_COMMON_INIT(ret, MSCM_SYN_DEFUN, file, line);
    ret->param_names = param_names;
    ret->body = body;
    strncpy(ret->func_name, func_name.start, func_name.len);
    ret->func_name[func_name.len] = 0;
    return (mscm_syntax_node)ret;
}

void mscm_free_syntax_node(mscm_syntax_node node) {
    while (node) {
        mscm_syntax_node current = node;
        node = node->next;

        switch (current->kind) {
            case MSCM_SYN_VALUE: {
                mscm_value_node *value = (mscm_value_node*)current;
                mscm_free_value_deep(value->value);
                break;
            }
            case MSCM_SYN_APPLY: {
                mscm_apply *apply = (mscm_apply*)current;
                mscm_free_syntax_node(apply->callee);
                mscm_free_syntax_node(apply->args);
                break;
            }
            case MSCM_SYN_BEGIN: {
                mscm_begin *begin = (mscm_begin*)current;
                mscm_free_syntax_node(begin->content);
                break;
            }
            case MSCM_SYN_LAMBDA: case MSCM_SYN_DEFUN: {
                mscm_func_def *fndef = (mscm_func_def*)current;
                mscm_ident *param_names = fndef->param_names;
                mscm_free_syntax_node((mscm_syntax_node)param_names);
                mscm_free_syntax_node(fndef->body);
                break;
            }
            case MSCM_SYN_COND: {
                mscm_cond *cond = (mscm_cond*)current;
                mscm_free_syntax_node(cond->cond_list);
                mscm_free_syntax_node(cond->then_list);
                break;
            }
            case MSCM_SYN_IF: {
                mscm_if *if_ = (mscm_if*)current;
                mscm_free_syntax_node(if_->then);
                mscm_free_syntax_node(if_->otherwise);
                break;
            }
            case MSCM_SYN_DEFVAR: {
                mscm_defvar *defvar = (mscm_defvar*)current;
                mscm_free_syntax_node(defvar->init);
                break;
            }
        }

        free(current);
    }
}
