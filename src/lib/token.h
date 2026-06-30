#ifndef WORDMAKE_TOKEN_H
#define WORDMAKE_TOKEN_H
#include <stdint.h>
#include "common.h"


typedef enum {
    T_EOF,
    T_AT,        // @
    T_EQ,        // =
    T_COLON,     // :
    T_LBRACK,    // [
    T_RBRACK,    // ]
    T_LPAREN,    // (
    T_RPAREN,    // )
    T_LCURLY,    // {
    T_RCURLY,    // }
    T_COMMA,     // ,
    T_PLUS,      // +
    T_MINUS,     // -
    T_DOT,       // .
    T_UNDERSCORE,// _
    T_BANG,      // !
    T_QMARK,     // ?
    T_GREATER,   // >
    T_LESSER,    // <
    T_SLASH,     // /
    T_BACKSLASH, // '\'
    T_PIPE,      // |
    T_AMP,       // &
    T_DOLLAR,    // $
    T_STAR,      // *
    T_SEMI,      // ;
    T_HASH,      // #
    T_CIRCUM,    // ^
    T_PERCENT,   // %
    T_TILDE,     // ~
    T_GRAVE,     // `
    T_TICK,      // '
    T_QUOTE,     // "

    T_NUMBER,    // numeric literal (digits)
    T_ID,        // identifier (one-or-more letters)
    T_ID_SHORT   // identifier (short)
} TokenKind;

// ReSharper disable once CppNotAllPathsReturnValue
static inline const char *token_kind_name(TokenKind k) {
    // ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
    switch (k) {
        case T_EOF:        return "T_EOF";
        case T_AT:         return "T_AT";
        case T_EQ:         return "T_EQ";
        case T_COLON:      return "T_COLON";
        case T_LBRACK:     return "T_LBRACK";
        case T_RBRACK:     return "T_RBRACK";
        case T_LPAREN:     return "T_LPAREN";
        case T_RPAREN:     return "T_RPAREN";
        case T_LCURLY:     return "T_LCURLY";
        case T_RCURLY:     return "T_RCURLY";
        case T_COMMA:      return "T_COMMA";
        case T_PLUS:       return "T_PLUS";
        case T_MINUS:      return "T_MINUS";
        case T_DOT:        return "T_DOT";
        case T_UNDERSCORE: return "T_UNDERSCORE";
        case T_BANG:       return "T_BANG";
        case T_QMARK:      return "T_QMARK";
        case T_GREATER:    return "T_GREATER";
        case T_LESSER:     return "T_LESSER";
        case T_SLASH:      return "T_SLASH";
        case T_BACKSLASH:  return "T_BACKSLASH";
        case T_PIPE:       return "T_PIPE";
        case T_AMP:        return "T_AMP";
        case T_DOLLAR:     return "T_DOLLAR";
        case T_STAR:       return "T_STAR";
        case T_SEMI:       return "T_SEMI";
        case T_HASH:       return "T_HASH";
        case T_CIRCUM:     return "T_CIRCUM";
        case T_PERCENT:    return "T_PERCENT";
        case T_TILDE:      return "T_TILDE";
        case T_GRAVE:      return "T_GRAVE";
        case T_TICK:       return "T_TICK";
        case T_QUOTE:      return "T_QUOTE";

        case T_NUMBER:     return "T_NUMBER";
        case T_ID:         return "T_ID";
        case T_ID_SHORT:   return "T_ID_SHORT";
    }
}


typedef union {
    uint32_t number; // for NUMBER, 0 for anything else
    char *string;    // for ID
    char short_string[8]; // for ID_SHORT
} TokenData;


typedef struct {
    TokenData value;
    TokenKind kind;
    size_t line;             // optional: source line
    size_t column;           // optional: source column
} Token; // Unused?


typedef struct {
    TokenData *value;
    TokenKind *kind;
    size_t *line;
    size_t *column;
} TokenArray; // SOA


typedef struct {
    TokenArray data;
    size_t capacity;
    size_t size;
} TokenList; // SOA


static inline int tokenlist_init(TokenList *tl) {

    TokenData *nv = malloc(INITIAL_TOKENS_CAP * sizeof(*nv));
    if (!nv) return ALLOC_ERR;
    tl->data.value = nv;

    TokenKind *nk = malloc(INITIAL_TOKENS_CAP * sizeof(*nk));
    if (!nk) return ALLOC_ERR;
    tl->data.kind = nk;

    size_t *nl = malloc(INITIAL_TOKENS_CAP * sizeof(*nl));
    if (!nl) return ALLOC_ERR;
    tl->data.line = nl;

    size_t *nc = malloc(INITIAL_TOKENS_CAP * sizeof(*nc));
    if (!nc) return ALLOC_ERR;
    tl->data.column = nc;

    tl->size = 0;
    tl->capacity = INITIAL_TOKENS_CAP;
    return OK;
}

static inline int tokenlist_grow(TokenList *tl) {

    tl->capacity *= 2;

    /* realloc each array */
    TokenData *nv = realloc(tl->data.value, tl->capacity * sizeof(*nv));
    if (!nv) return ALLOC_ERR;
    tl->data.value = nv;

    TokenKind *nk = realloc(tl->data.kind, tl->capacity * sizeof(*nk));
    if (!nk) return ALLOC_ERR;
    tl->data.kind = nk;

    size_t *nl = realloc(tl->data.line, tl->capacity * sizeof(*nl));
    if (!nl) return ALLOC_ERR;
    tl->data.line = nl;

    size_t *nc = realloc(tl->data.column, tl->capacity * sizeof(*nc));
    if (!nc) return ALLOC_ERR;
    tl->data.column = nc;

    return OK;
}

static inline int tokenlist_append(
    TokenList *tl, FILE *log,
    const TokenData value, TokenKind kind,
    size_t line, size_t column
    ) {
    if (tl->size + 1 > tl->capacity) {
        if (tokenlist_grow(tl) == ALLOC_ERR) return ALLOC_ERR;
    }
    tl->data.value[tl->size] = value; /* shallow copy; adjust if TokenData contains pointers */
    tl->data.kind[tl->size] = kind;
    tl->data.line[tl->size] = line;
    tl->data.column[tl->size] = column;
    tl->size++;

    if (log != NULL) {
        fprintf(log, "(%llu, %llu) %s", line, column, token_kind_name(kind));

        if (kind == T_NUMBER) {
            fprintf(log, " '%d'", value.number);
        } else if (kind == T_ID) {
            fprintf(log, " '%s'", value.string);
        }

        fprintf(log, "\n");
    }
#ifdef PRINT_LOG
    printf("(%llu, %llu) %s", line, column, token_kind_name(kind));

    if (kind == T_NUMBER) {
        printf(" '%d'", value.number);
    } else if (kind == T_ID) {
        printf(" '%s'", value.string);
    }
    printf("\n");

#endif




    return OK;
}

static inline void tokenlist_free(TokenList *tl) {
    if (!tl) return;

    for (size_t i = 0; i < tl->size; i++) {
        if (tl->data.kind[i] == T_ID) {
            free(tl->data.value[i].string);
        }
    }

    free(tl->data.value);
    free(tl->data.kind);
    free(tl->data.line);
    free(tl->data.column);
    tl->data.value = NULL;
    tl->data.kind = NULL;
    tl->data.line = NULL;
    tl->data.column = NULL;
    tl->size = 0;
    tl->capacity = 0;
}


#endif //WORDMAKE_TOKEN_H
