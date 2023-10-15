#include <assert.h>
#include <stdint.h>

#include "parse.h"
#include "syntax.h"
#include "tokenise.h"
#include "value.h"
#include "util.h"

static mscm_syntax_node parse_item(tokenise_ctx *tokeniser);
static mscm_syntax_node parse_list_alike(tokenise_ctx *tokeniser,
                                         uint8_t paren_kind);
static mscm_syntax_node parse_item_list(tokenise_ctx *tokeniser,
                                        uint8_t rparen_kind,
                                        bool *ok);
static mscm_syntax_node parse_define(tokenise_ctx *tokeniser,
                                     uint8_t rparen_kind);
static mscm_syntax_node parse_defvar(tokenise_ctx *tokeniser,
                                     uint8_t rparen_kind);
static mscm_syntax_node parse_defun(tokenise_ctx *tokeniser,
                                    uint8_t rparen_kind);
static mscm_syntax_node parse_lambda(tokenise_ctx *tokeniser,
                                     uint8_t rparen_kind);
static mscm_syntax_node parse_cond(tokenise_ctx *tokeniser,
                                   uint8_t rparen_kind);
static mscm_syntax_node parse_if(tokenise_ctx *tokeniser,
                                 uint8_t rparen_kind);
static mscm_value parse_quoted_list(tokenise_ctx *tokeniser, bool *ok);
static mscm_value parse_quoted_list_impl(tokenise_ctx *tokeniser,
                                         uint8_t rparen_kind,
                                         bool *ok);
static mscm_ident* parse_ident_list(tokenise_ctx *tokeniser);
static void unexpected_token(token t);
static char const *tk_string(uint8_t tk);

mscm_syntax_node mscm_parse(char const *file, char const *content) {
    tokenise_ctx ctx = (tokenise_ctx) {
        content, file, 1,
        false,
        { TK_EOI, { 0 }, file, 1 }
    };

    mscm_syntax_node ret = 0;
    mscm_syntax_node current = 0;

    while (peek_token(&ctx).tk != TK_EOI) {
        mscm_syntax_node node = parse_item(&ctx);
        if (!node) {
            mscm_free_syntax_node(ret);
            return 0;
        }

        if (current == 0) {
            ret = node;
            current = ret;
        }
        else {
            current->next = node;
            current = node;
        }
    }

    return ret;
}

static mscm_syntax_node parse_item(tokenise_ctx *tokeniser) {
    token t = get_token(tokeniser);
    switch (t.tk) {
        case TK_INT: {
            mscm_value value = mscm_make_int(t.value.i);
            return mscm_make_value_node(t.file, t.line, value);
        }
        case TK_FLOAT: {
            mscm_value value = mscm_make_float(t.value.f);
            return mscm_make_value_node(t.file, t.line, value);
        }
        case TK_STRING: {
            bool esc_ok;
            mscm_value value =
                mscm_make_string(t.value.s, true, &esc_ok);
            if (!esc_ok) {
                err_print(t.file, t.line,
                          "invalid escape sequence in string literal");
                return 0;
            }
            return mscm_make_value_node(t.file, t.line, value);
        }
        case TK_SYMBOL: {
            mscm_value value =
                mscm_make_string(t.value.s, false, 0);
            value->type = MSCM_TYPE_SYMBOL;
            return mscm_make_value_node(t.file, t.line, value);
        }
        case TK_IDENT: {
            return mscm_make_ident(t.file, t.line, t.value.s);
        }
        case TK_LBRACKET: case TK_RBRACKET:
        case TK_LPAREN: case TK_RPAREN: {
            return parse_list_alike(tokeniser, t.tk);
        }
        case TK_QUOTE: {
            bool ok;
            mscm_value value = parse_quoted_list(tokeniser, &ok);
            if (!ok) {
                return 0;
            }
            return mscm_make_value_node(t.file, t.line, value);
        }
    }

    unexpected_token(t);
    return 0;
}

static mscm_syntax_node parse_list_alike(tokenise_ctx *tokeniser,
                                         uint8_t paren_kind) {
    uint8_t rparen_kind =
        paren_kind == TK_LBRACKET ? TK_RBRACKET : TK_RPAREN;
    if (peek_token(tokeniser).tk == rparen_kind) {
        get_token(tokeniser);
        return 0;
    }

    token t = peek_token(tokeniser);
    if (token_is_ident(t, "define")) {
        return parse_define(tokeniser, rparen_kind);
    }
    else if (token_is_ident(t, "lambda")) {
        return parse_lambda(tokeniser, rparen_kind);
    }
    else if (token_is_ident(t, "cond")) {
        return parse_cond(tokeniser, rparen_kind);
    }
    else if (token_is_ident(t, "if")) {
        return parse_if(tokeniser, rparen_kind);
    }
    else if (token_is_ident(t, "begin")) {
        token begin = get_token(tokeniser);
        bool ok;
        mscm_syntax_node body =
            parse_item_list(tokeniser, rparen_kind, &ok);
        if (!ok) {
            return 0;
        }
        return mscm_make_begin(begin.file, begin.line, body);
    }
    else {
        mscm_syntax_node callee = parse_item(tokeniser);
        if (!callee) {
            return 0;
        }

        bool ok;
        mscm_syntax_node args =
            parse_item_list(tokeniser, rparen_kind, &ok);
        if (!ok) {
            mscm_free_syntax_node(callee);
            return 0;
        }

        return mscm_make_apply(tokeniser->file, tokeniser->line,
                               callee, args);
    }
}

static mscm_syntax_node parse_item_list(tokenise_ctx *tokeniser,
                                        uint8_t rparen_kind,
                                        bool *ok) {
    mscm_syntax_node ret = 0, current = 0;
    while (peek_token(tokeniser).tk != rparen_kind
           && peek_token(tokeniser).tk != TK_EOI) {
        mscm_syntax_node node = parse_item(tokeniser);
        if (!node) {
            mscm_free_syntax_node(ret);
            if (ok) {
                *ok = false;
            }
            return 0;
        }

        if (current == 0) {
            ret = node;
            current = ret;
        }
        else {
            current->next = node;
            current = node;
        }
    }

    if (peek_token(tokeniser).tk == TK_EOI) {
        err_print(tokeniser->file, tokeniser->line,
                  "unexpected end of input");
        mscm_free_syntax_node(ret);
        if (ok) {
            *ok = false;
        }
        return 0;
    }

    get_token(tokeniser);
    if (ok) {
        *ok = true;
    }
    return ret;
}

static mscm_syntax_node parse_define(tokenise_ctx *tokeniser,
                                     uint8_t rparen_kind) {
    token define = get_token(tokeniser);
    assert(token_is_ident(define, "define"));
    (void)define;

    token t = peek_token(tokeniser);
    if (t.tk == TK_IDENT) {
        return parse_defvar(tokeniser, rparen_kind);
    }
    else if (t.tk == TK_LBRACKET || t.tk == TK_LPAREN) {
        return parse_defun(tokeniser, rparen_kind);
    }
    else {
        unexpected_token(t);
        return 0;
    }
}

static mscm_syntax_node parse_defvar(tokenise_ctx *tokeniser,
                                     uint8_t rparen_kind) {
    token ident = get_token(tokeniser);
    if (ident.tk != TK_IDENT) {
        unexpected_token(ident);
        return 0;
    }

    mscm_syntax_node init = parse_item(tokeniser);
    if (!init) {
        return 0;
    }

    token t_end = get_token(tokeniser);
    if (t_end.tk != rparen_kind) {
        unexpected_token(t_end);
        return 0;
    }

    return mscm_make_defvar(ident.file, ident.line, ident.value.s, init);
}

static mscm_syntax_node parse_defun(tokenise_ctx *tokeniser,
                                    uint8_t rparen_kind) {
    token lp = get_token(tokeniser);
    if (lp.tk != TK_LPAREN && lp.tk != TK_LBRACKET) {
        unexpected_token(lp);
        return 0;
    }

    token fn_name = get_token(tokeniser);
    if (fn_name.tk != TK_IDENT) {
        unexpected_token(fn_name);
        return 0;
    }

    mscm_ident *param_names = parse_ident_list(tokeniser);

    token rp = get_token(tokeniser);
    if (!(lp.tk == TK_LPAREN && rp.tk == TK_RPAREN) &&
        !(lp.tk == TK_LBRACKET && rp.tk == TK_RBRACKET)) {
        unexpected_token(rp);
        return 0;
    }

    mscm_syntax_node body = parse_item_list(tokeniser, rparen_kind, 0);
    if (!body) {
        mscm_free_syntax_node((mscm_syntax_node)param_names);
        err_print(fn_name.file, fn_name.line,
                  "expected function body");
        return 0;
    }

    return mscm_make_func_def(tokeniser->file, tokeniser->line,
                              fn_name.value.s, param_names, body);
}

static mscm_syntax_node parse_lambda(tokenise_ctx *tokeniser,
                                     uint8_t rparen_kind) {
    token lambda = get_token(tokeniser);

    token lp = get_token(tokeniser);
    if (lp.tk != TK_LPAREN && lp.tk != TK_LBRACKET) {
        unexpected_token(lp);
        return 0;
    }

    mscm_ident *param_names = parse_ident_list(tokeniser);

    token rp = get_token(tokeniser);
    if (!(lp.tk == TK_LPAREN && rp.tk == TK_RPAREN) &&
        !(lp.tk == TK_LBRACKET && rp.tk == TK_RBRACKET)) {
        unexpected_token(rp);
        return 0;
    }

    mscm_syntax_node body = parse_item_list(tokeniser, rparen_kind, 0);
    if (!body) {
        mscm_free_syntax_node((mscm_syntax_node)param_names);
        err_print(lambda.file, lambda.line,
                  "expected function body");
        return 0;
    }

    return mscm_make_lambda(lambda.file, lambda.line,
                            param_names, body);
}

static mscm_syntax_node parse_cond(tokenise_ctx *tokeniser,
                                   uint8_t rparen_kind) {
    token cond = get_token(tokeniser);
    assert(token_is_ident(cond, "cond"));

    mscm_syntax_node cond_list = 0;
    mscm_syntax_node cond_list_current = 0;
    mscm_syntax_node then_list = 0;
    mscm_syntax_node then_list_current = 0;
    while (peek_token(tokeniser).tk != rparen_kind
           && peek_token(tokeniser).tk != TK_EOI) {
        token lp = get_token(tokeniser);
        if (lp.tk != TK_LPAREN && lp.tk != TK_LBRACKET) {
            unexpected_token(lp);
            mscm_free_syntax_node(cond_list);
            mscm_free_syntax_node(then_list);
            return 0;
        }

        mscm_syntax_node cond_item = parse_item(tokeniser);
        if (!cond_item) {
            mscm_free_syntax_node(cond_list);
            mscm_free_syntax_node(then_list);
            return 0;
        }

        mscm_syntax_node then_item = parse_item(tokeniser);
        if (!then_item) {
            mscm_free_syntax_node(cond_item);
            mscm_free_syntax_node(cond_list);
            mscm_free_syntax_node(then_list);
            return 0;
        }

        if (cond_list == 0) {
            cond_list = cond_item;
            then_list = then_item;
            cond_list_current = cond_list;
            then_list_current = then_list;
        }
        else {
            cond_list_current->next = cond_item;
            then_list_current->next = then_item;
            cond_list_current = cond_item;
            then_list_current = then_item;
        }

        token rp = get_token(tokeniser);
        if (!(lp.tk == TK_LPAREN && rp.tk == TK_RPAREN) &&
            !(lp.tk == TK_LBRACKET && rp.tk == TK_RBRACKET)) {
            unexpected_token(rp);
            mscm_free_syntax_node(cond_list);
            mscm_free_syntax_node(then_list);
            return 0;
        }
    }

    if (peek_token(tokeniser).tk == TK_EOI) {
        err_print(tokeniser->file, tokeniser->line,
                  "unexpected end of input");
        mscm_free_syntax_node(cond_list);
        mscm_free_syntax_node(then_list);
        return 0;
    }

    get_token(tokeniser);
    return mscm_make_cond(cond.file, cond.line, cond_list, then_list);
}

static mscm_syntax_node parse_if(tokenise_ctx *tokeniser,
                                 uint8_t rparen_kind) {
    token if_ = get_token(tokeniser);
    assert(token_is_ident(if_, "if"));

    mscm_syntax_node cond = parse_item(tokeniser);
    if (!cond) {
        return 0;
    }

    mscm_syntax_node then = parse_item(tokeniser);
    if (!then) {
        mscm_free_syntax_node(cond);
        return 0;
    }

    if (peek_token(tokeniser).tk == rparen_kind) {
        get_token(tokeniser);
        return mscm_make_if(if_.file, if_.line, cond, then, 0);
    }

    mscm_syntax_node otherwise = parse_item(tokeniser);
    if (!otherwise) {
        mscm_free_syntax_node(cond);
        mscm_free_syntax_node(then);
        return 0;
    }

    token rp = get_token(tokeniser);
    if (rp.tk != rparen_kind) {
        unexpected_token(rp);
        mscm_free_syntax_node(cond);
        mscm_free_syntax_node(then);
        mscm_free_syntax_node(otherwise);
        return 0;
    }

    return mscm_make_if(if_.file, if_.line, cond, then, otherwise);
}

static mscm_value parse_quoted_list(tokenise_ctx *tokeniser, bool *ok) {
    token lp = get_token(tokeniser);
    if (lp.tk != TK_LPAREN && lp.tk != TK_LBRACKET) {
        unexpected_token(lp);
        *ok = false;
        return 0;
    }

    uint8_t rp_kind = lp.tk == TK_LPAREN ? TK_RPAREN : TK_RBRACKET;
    return parse_quoted_list_impl(tokeniser, rp_kind, ok);
}

static mscm_value parse_quoted_list_impl(tokenise_ctx *tokeniser,
                                         uint8_t rparen_kind,
                                         bool *ok) {
    mscm_value ret = 0;
    mscm_pair *current = 0;

    while (peek_token(tokeniser).tk != rparen_kind
           && peek_token(tokeniser).tk != TK_EOI) {
        mscm_value item = 0;
        token t = get_token(tokeniser);
        switch (t.tk) {
            case TK_INT: {
                item = mscm_make_int(t.value.i);
                break;
            }
            case TK_FLOAT: {
                item = mscm_make_float(t.value.f);
                break;
            }
            case TK_STRING: {
                bool esc_ok;
                item = mscm_make_string(t.value.s, true, &esc_ok);
                if (!esc_ok) {
                    err_print(t.file, t.line,
                              "invalid escape sequence in string literal");
                    *ok = false;
                    mscm_free_value_deep(ret);
                    return 0;
                }
                break;
            }
            case TK_SYMBOL: case TK_IDENT: {
                item = mscm_make_string(t.value.s, false, 0);
                item->type = MSCM_TYPE_SYMBOL;
                break;
            }
            case TK_LBRACKET: case TK_RBRACKET: {
                item = parse_quoted_list_impl(tokeniser, t.tk, ok);
                if (!item) {
                    *ok = false;
                    return 0;
                }
                break;
            }
            case TK_QUOTE: {
                err_print(t.file, t.line, "quotes cannot be nested");
                *ok = false;
                mscm_free_value_deep(ret);
                return 0;
            }
            default: {
                unexpected_token(t);
                *ok = false;
                return 0;
            }
        }

        if (current == 0) {
            ret = mscm_make_pair(item, 0);
            current = (mscm_pair*)ret;
        }
        else {
            current->snd = mscm_make_pair(item, 0);
            current = (mscm_pair*)current->snd;
        }
    }

    if (peek_token(tokeniser).tk == TK_EOI) {
        err_print(tokeniser->file, tokeniser->line,
                  "unexpected end of input");
        *ok = false;
        mscm_free_value_deep(ret);
        return 0;
    }

    get_token(tokeniser);
    *ok = true;
    return ret;
}

static mscm_ident* parse_ident_list(tokenise_ctx *tokeniser) {
    mscm_syntax_node ret = 0;
    mscm_syntax_node current = 0;

    while (peek_token(tokeniser).tk == TK_IDENT) {
        token t = get_token(tokeniser);
        mscm_syntax_node node = mscm_make_ident(t.file, t.line, t.value.s);
        if (!node) {
            mscm_free_syntax_node(ret);
            return 0;
        }

        if (current == 0) {
            ret = node;
            current = ret;
        }
        else {
            current->next = node;
            current = node;
        }
    }

    return (mscm_ident*)ret;
}

static void unexpected_token(token t) {
    err_printf(t.file, t.line, "unexpected %s",
               tk_string(t.tk));
}

static char const *tk_string(uint8_t tk) {
    switch (tk) {
        case TK_INT: return "integer";
        case TK_FLOAT: return "float";
        case TK_STRING: return "string";
        case TK_SYMBOL: return "symbol";
        case TK_IDENT: return "identifier";
        case TK_LBRACKET: return "`[`";
        case TK_RBRACKET: return "`]`";
        case TK_LPAREN: return "`(`";
        case TK_RPAREN: return "`)`";
        case TK_QUOTE: return "`'`";
        case TK_EOI: return "end of input";
        default:
            assert(false);
            return "unknown token";
    }
}
