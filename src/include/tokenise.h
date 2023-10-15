#ifndef MINI_SCHEME_TOKENISE_H
#define MINI_SCHEME_TOKENISE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "slice.h"

enum {
    TK_INT      = 0,
    TK_FLOAT    = 1,
    TK_IDENT    = 2,
    TK_STRING   = 3,
    TK_SYMBOL   = 4,
    TK_QUOTE    = 5,
    TK_LBRACKET = 6,
    TK_RBRACKET = 7,
    TK_LPAREN   = 8,
    TK_RPAREN   = 9,

    TK_EOI      = 10
};

typedef struct {
    uint8_t tk;
    union {
        int64_t i;
        double f;
        mscm_slice s;
    } value;

    char const *file;
    size_t line;
} token;

typedef struct {
    char const *cursor;
    char const *file;
    size_t line;

    bool has_unget;
    token unget;
} tokenise_ctx;

token get_token(tokenise_ctx *ctx);
token peek_token(tokenise_ctx *ctx);
void unget_token(tokenise_ctx *ctx, token t);

bool token_is_ident(token t, char const *ident);

#endif /* MINI_SCHEME_TOKENISE_H */
