//
// Created by Swarup on 2026-06-18.
//

#ifndef WORDMAKE_LEXER_H
#define WORDMAKE_LEXER_H

#include <ctype.h>
#include <stdio.h>
#include "mymap.h"
#include "common.h"
#include "token.h"

typedef struct {
    const char *input; // mem-mapped file
    FILE *log;

    TokenList token_list; // SOA list of tokens

    size_t line; // line #
    size_t column; // column index from line start

    size_t size; // size of input
    size_t position; // position in input
} Lexer;

static Lexer generate_lexer(MyMap input, FILE *logf, TokenList *tl) {
    Lexer lex;
    lex.input = input.addr;
    lex.log = logf;
    lex.token_list = *tl;

    lex.line = 0;
    lex.column = 0;
    lex.size = input.size;
    lex.position = 0;
    return lex;
}


static inline char peek(Lexer *lex) {
    return lex->input[lex->position];
}

static inline const char *peekerN(Lexer *lex, size_t position) {
    return lex->input + position;
}

static inline int end(Lexer *lex) {
    return lex->position == lex->size;
}


// do not check for end
static inline int advance(Lexer *lex) {

    lex->position++;

    if (isLF(peek(lex))) {
        lex->line++;
        lex->column = 1;
    } else {
        lex->column++;;

    }

    return OK;
}

static inline void unsafe_backup(Lexer *lex) {
    lex->position--;
}

static inline void skip_whitespace(Lexer *lex) {
    while (isWhitespace(peek(lex))) {
        advance(lex);
    }
}

static inline int skip_line(Lexer *lex) {
    while (!isLF(peek(lex))) {
        if (end(lex)) return EOF_ERR;
        advance(lex);
    }
    return OK;
}

#define UINT_FROM_STRING_T uint32_t
static inline uint32_t uint_from_string(Lexer *lex) {
    uint32_t num = 0;
    // digits
    while (isDigit(peek(lex))) {
        num = (num * 10) + (peek(lex) - '0');
        advance(lex);
    }

    return num;
}

#define TOKEN$(lex, kind, data) \
    tokenlist_append(&lex->token_list, lex->log, data, kind, lex->line, lex->column); \
    advance(lex)

#define NIL (TokenData) {.number = 0}

#define CASE$(x, tok) \
    case x: do {\
    tokenlist_append(&lex->token_list, lex->log, NIL, tok, lex->line, lex->column); \
    advance(lex); \
    } while (0)


static int inline lexer(Lexer *lex, FILE *log) {
    for (;;) {
        skip_whitespace(lex);

        if (end(lex)) return OK;

        char c = peek(lex);
        printf("'%c'", peek(lex));
        switch (c) {
            CASE$('@', T_AT); continue;
            CASE$('=', T_EQ); continue;
            CASE$(':', T_COLON); continue;
            CASE$('[', T_LBRACK); continue;
            CASE$(']', T_RBRACK); continue;
            CASE$('(', T_LPAREN); continue;
            CASE$(')', T_RPAREN); continue;
            CASE$('{', T_LCURLY); continue;
            CASE$('}', T_RCURLY); continue;
            CASE$(',', T_COMMA); continue;
            CASE$('+', T_PLUS); continue;
            CASE$('-', T_MINUS); continue;
            CASE$('.', T_DOT); continue;
            CASE$('_', T_UNDERSCORE); continue;
            CASE$('!', T_BANG); continue;
            CASE$('?', T_QMARK); continue;
            CASE$('>', T_GREATER); continue;
            CASE$('<', T_LESSER); continue;
            CASE$('/', T_SLASH); continue;
            CASE$('\\', T_BACKSLASH); continue;
            CASE$('|', T_PIPE); continue;
            CASE$('&', T_AMP); continue;
            CASE$('$', T_DOLLAR); continue;
            CASE$('*', T_STAR); continue;
            CASE$('^', T_CIRCUM); continue;
            CASE$('%', T_PERCENT); continue;
            CASE$('~', T_TILDE); continue;
            CASE$('`', T_GRAVE); continue;
            CASE$('\'', T_TICK); continue;
            CASE$('"', T_QUOTE); continue;
            default: break;
        }


        if (c == '#' || c == ';') {
            // TODO: comments end at tokenizer
            // TODO: insert T_EOL when "\n" reached instead.

            int err = TOKEN$(lex, c == '#' ? T_HASH : T_SEMI, NIL);
            if (err == ALLOC_ERR) return ALLOC_ERR;

            int err2 = skip_line(lex);
            if (err2 == EOF_ERR) return OK;

            continue;
        }

        if (isdigit(c)) {
            uint32_t NUMBER = 0;
            while (isDigit(peek(lex))) {
                if (end(lex)) return OK;
                NUMBER = (NUMBER * 10) + (peek(lex) - '0');
                advance(lex);
            }

            int app = TOKEN$(lex, T_NUMBER, (TokenData)NUMBER);
            if (app == ALLOC_ERR) return ALLOC_ERR;

            unsafe_backup(lex); // safe
            if (NUMBER == 0) return NUMBER_ERR;
            continue;
        }


        if (isAlpha(c) || (uint8_t)c > 127) {
            // TODO: alpha is anything not configured chars or digits
            // TODO: add utf8 handling
            size_t initial_pos = lex->position;

            while (isAlpha(peek(lex)) || (uint8_t)peek(lex) > 127) {
                if (end(lex)) return EOF_ERR;
                advance(lex);
            }

            size_t len = lex->position - initial_pos;
            char *id = malloc(sizeof(char) * (lex->position - initial_pos + 1));
            if (!id) return ALLOC_ERR;

            memcpy(id, peekerN(lex, initial_pos), len);
            id[len] = '\0';

            int app = TOKEN$(lex, T_ID, (TokenData)id);
            if (app == ALLOC_ERR) return ALLOC_ERR;

            unsafe_backup(lex); // safe
            continue;
        }
        return UNKNOWN_TOKEN_ERR;

    }
}


#endif //WORDMAKE_LEXER_H
