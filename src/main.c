//
// Created by on 2026-06-18.
//

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "common.h"
#include "mymap.h"
#include "token.h"
#include "config.h"

int main(int argc, char **argv) {
    // edit this to be -f files
    // add a REPL option.
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input-file> <lex-log-file>\n", argv[0]);
        return ARG_ERR;
    }

    MyMap input;

    if (!mymap_map_file(&input, argv[1], MyProt_Read)) {
        fprintf(stderr, "Error: cannot open input file '%s' for read\n",
                argv[1]);
        return READ_ERR;
    }


    FILE *logf;

    if (strcmp(argv[2], "-") != 0) {
        logf = fopen(argv[2], "w");
        if (!logf) {
            fprintf(stderr, "Error: cannot open log file '%s' for append\n",
                    argv[2]);
            mymap_unmap(&input);
            return READ_ERR;
        }
    } else logf = NULL;


    TokenList tl, *token_list = &tl;
    int initiated = tokenlist_init(token_list);
    if (initiated != 0) return initiated;

    Lexer lex = generate_lexer(input, logf, token_list);

    const int lexed = lexer(&lex, logf);
    if (lexed != OK) {
        mymap_unmap(&input);
        fclose(logf);
        tokenlist_free(token_list);
        return lexed;
    }


    mymap_unmap(&input);
    fclose(logf);
    tokenlist_free(token_list);
    return OK;
}