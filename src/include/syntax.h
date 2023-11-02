#ifndef MINI_SCHEME_SYNTAX_H
#define MINI_SCHEME_SYNTAX_H

#include <stddef.h>
#include <stdint.h>

#include "slice.h"

typedef struct st_mscm_value_base *mscm_value;

#define MSCM_SYNTAX_NODE_COMMON \
    uint8_t kind; \
    char const *file; \
    size_t line; \
    struct st_mscm_syntax *next;

enum {
    /* syntax nodes that are expressions */
    MSCM_SYN_VALUE   = 0,
    MSCM_SYN_IDENT   = 1,
    MSCM_SYN_APPLY  = 2,
    MSCM_SYN_BEGIN   = 3,
    MSCM_SYN_LAMBDA  = 4,
    MSCM_SYN_COND    = 5,
    MSCM_SYN_IF      = 6,
    MSCM_SYN_LOOP    = 7,
    MSCM_SYN_BREAK   = 8,

    /* syntax nodes that are not expressions */
    MSCM_SYN_DEFVAR  = 100,
    MSCM_SYN_DEFUN   = 101
};

typedef struct st_mscm_syntax {
    MSCM_SYNTAX_NODE_COMMON
} mscm_syntax_node_base, *mscm_syntax_node;

typedef struct {
    MSCM_SYNTAX_NODE_COMMON
    mscm_value value;
} mscm_value_node;

typedef struct {
    MSCM_SYNTAX_NODE_COMMON
    char ident[];
} mscm_ident;

typedef struct {
    MSCM_SYNTAX_NODE_COMMON
    mscm_syntax_node content;
} mscm_begin;

typedef struct {
    MSCM_SYNTAX_NODE_COMMON
    mscm_syntax_node callee;
    mscm_syntax_node args;
} mscm_apply;

typedef struct {
    MSCM_SYNTAX_NODE_COMMON
    mscm_syntax_node cond_list;
    mscm_syntax_node then_list;
} mscm_cond;

typedef struct {
    MSCM_SYNTAX_NODE_COMMON
    mscm_syntax_node cond;
    mscm_syntax_node then;
    mscm_syntax_node otherwise;
} mscm_if;

typedef struct {
    MSCM_SYNTAX_NODE_COMMON
    mscm_syntax_node init;
    char var_name[];
} mscm_defvar;

/* lambda and defun shares one structure */
typedef struct st_mscm_func_def{
    MSCM_SYNTAX_NODE_COMMON
    mscm_ident *param_names;
    mscm_syntax_node body;
    char func_name[];
} mscm_func_def;

mscm_syntax_node mscm_make_value_node(char const *file,
                                      size_t line,
                                      mscm_value value);

mscm_syntax_node mscm_make_ident(char const *file,
                                 size_t line,
                                 mscm_slice ident);

mscm_syntax_node mscm_make_apply(char const *file,
                                 size_t line,
                                 mscm_syntax_node callee,
                                 mscm_syntax_node args);

mscm_syntax_node mscm_make_begin(char const *file,
                                 size_t line,
                                 mscm_syntax_node content);

mscm_syntax_node mscm_make_lambda(char const *file,
                                  size_t line,
                                  mscm_ident *param_names,
                                  mscm_syntax_node body);

mscm_syntax_node mscm_make_cond(char const *file,
                                size_t line,
                                mscm_syntax_node cond_list,
                                mscm_syntax_node then_list);

mscm_syntax_node mscm_make_if(char const *file,
                              size_t line,
                              mscm_syntax_node cond,
                              mscm_syntax_node then,
                              mscm_syntax_node otherwise);

mscm_syntax_node mscm_make_defvar(char const *file,
                                  size_t line,
                                  mscm_slice var_name,
                                  mscm_syntax_node init);

mscm_syntax_node mscm_make_func_def(char const *file,
                                    size_t line,
                                    mscm_slice func_name,
                                    mscm_ident *param_names,
                                    mscm_syntax_node body);

void mscm_free_syntax_node(mscm_syntax_node node);

#endif /* MINI_SCHEME_SYNTAX_H */
