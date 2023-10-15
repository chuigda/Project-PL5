#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "tokenise.h"
#include "util.h"

#define TOKEN0(TT) (token) { TT, { 0 }, ctx->file, ctx->line }

static bool maybe_skip_whitespace(tokenise_ctx *ctx);
static bool maybe_skip_comment(tokenise_ctx *ctx);
static token tokenise_string(tokenise_ctx *ctx);
static token tokenise_num(tokenise_ctx *ctx);
static token tokenise_ident(tokenise_ctx *ctx);
static bool is_ident_char(char ch);

token get_token(tokenise_ctx *ctx) {
    if (ctx->has_unget) {
        if (ctx->unget.tk != TK_EOI) {
            ctx->has_unget = false;
        }
        return ctx->unget;
    }

    while (maybe_skip_whitespace(ctx) || maybe_skip_comment(ctx)) {;}

    if (ctx->cursor[0] == '\0') {
        return TOKEN0(TK_EOI);
    }

    switch (ctx->cursor[0]) {
        case '(': ctx->cursor += 1; return TOKEN0(TK_LPAREN);
        case ')': ctx->cursor += 1; return TOKEN0(TK_RPAREN);
        case '[': ctx->cursor += 1; return TOKEN0(TK_LBRACKET);
        case ']': ctx->cursor += 1; return TOKEN0(TK_RBRACKET);
        case '"': return tokenise_string(ctx);
        case '\'':
            if (is_ident_char(ctx->cursor[1])) {
                ctx->cursor += 1;
                token ident = tokenise_ident(ctx);
                ident.tk = TK_SYMBOL;
                return ident;
            }
            else {
                ctx->cursor += 1;
                return TOKEN0(TK_QUOTE);
            }
        case '+': case '-':
            if (isdigit(ctx->cursor[1])) {
                return tokenise_num(ctx);
            }
            /* fallthrough */
        default:
            /* fallthrough*/
            ;
    }

    if (isdigit(ctx->cursor[0])) {
        return tokenise_num(ctx);
    }
    else if (is_ident_char(ctx->cursor[0])) {
        return tokenise_ident(ctx);
    }
    else {
        err_print(ctx->file, ctx->line, "invalid character");
        return TOKEN0(TK_EOI);
    }
}

token peek_token(tokenise_ctx *ctx) {
    if (ctx->has_unget) {
        return ctx->unget;
    }

    token t = get_token(ctx);
    unget_token(ctx, t);
    return t;
}

void unget_token(tokenise_ctx *ctx, token t) {
    assert(!ctx->has_unget);
    ctx->has_unget = true;
    ctx->unget = t;
}

bool token_is_ident(token t, char const *ident) {
    return t.tk == TK_IDENT &&
        t.value.s.len == strlen(ident) &&
        !strncmp(t.value.s.start, ident, t.value.s.len);
}

static bool maybe_skip_whitespace(tokenise_ctx *ctx) {
    bool skipped = false;
    while (isspace(ctx->cursor[0])) {
        if (ctx->cursor[0] == '\n') {
            ctx->line += 1;
        }
        ctx->cursor += 1;
        skipped = true;
    }
    return skipped;
}

static bool maybe_skip_comment(tokenise_ctx *ctx) {
    if (ctx->cursor[0] != ';') {
        return false;
    }

    while (ctx->cursor[0] != '\0' && ctx->cursor[0] != '\n') {
        ctx->cursor += 1;
    }
    return true;
}

static token tokenise_string(tokenise_ctx *ctx) {
    size_t start_line = ctx->line;

    assert(ctx->cursor[0] == '"');
    ctx->cursor += 1;

    char const *start = ctx->cursor;
    while (ctx->cursor[0] != '"' && ctx->cursor[0] != '\0') {
        if (ctx->cursor[0] == '\\') {
            if (ctx->cursor[1] == '\0') {
                err_print(ctx->file, ctx->line,
                          "unterminated string literal");
                return TOKEN0(TK_EOI);
            }
            ctx->cursor += 2;
        }
        else {
            ctx->cursor += 1;
        }
    }
    size_t len = ctx->cursor - start;

    if (ctx->cursor[0] != '"') {
        err_print(ctx->file, ctx->line,
                  "unterminated string literal");
        return TOKEN0(TK_EOI);
    }
    ctx->cursor += 1;

    token ret;
    ret.file = ctx->file;
    ret.line = start_line;
    ret.tk = TK_STRING;
    ret.value.s = (mscm_slice) { start, len };
    return ret;
}

static token tokenise_ident(tokenise_ctx *ctx) {
    char const *start = ctx->cursor;
    while (is_ident_char(ctx->cursor[0])) {
        ctx->cursor += 1;
    }
    size_t len = ctx->cursor - start;

    token ret;
    ret.file = ctx->file;
    ret.line = ctx->line;
    ret.tk = TK_IDENT;
    ret.value.s = (mscm_slice) { start, len };
    return ret;
}

static token tokenise_num(tokenise_ctx *ctx) {
    if (ctx->cursor[0] == '+') {
        ctx->cursor += 1;
    }

    char const *start = ctx->cursor;
    char *end;
    int64_t ivalue = strtoll(start, &end, 0);
    if (end[0] == '.' || end[0] == 'e') {
        double fvalue = strtod(start, &end);
        ctx->cursor = end;

        token ret;
        ret.file = ctx->file;
        ret.line = ctx->line;
        ret.tk = TK_FLOAT;
        ret.value.f = fvalue;
        return ret;
    }
    else {
        ctx->cursor = end;
        token ret;
        ret.file = ctx->file;
        ret.line = ctx->line;
        ret.tk = TK_INT;
        ret.value.i = ivalue;
        return ret;
    }
}

static bool is_ident_char(char ch) {
    uint8_t byte = *(uint8_t*)(&ch);
    if (byte >= 128) {
        /* if source code is encoded with UTF-8 or some other multibyte
         * encoding, this would well-support that */
        return true;
    }

    if (isalnum(ch)) {
        return true;
    }

    switch (ch) {
        case '+': case '-': case '*': case '/': case '%': case '^':
        case '&': case '_': case '=': case '?': case '!': case '<':
        case '>': case '{': case '}': case ':': case '|': case ',':
        case '.': case '~': case '@': case '$': case '\\':
            return true;
        default:
            return false;
    }
}
