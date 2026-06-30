//
// Created by WebCloud on 2026-06-22.
//

#ifndef WORDMAKE_CONFIG_H
#define WORDMAKE_CONFIG_H
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    None = 0,
    Brackets = 1 << 0,
    Parenthesis = 1<< 1,
    Braces = 1 << 2,
    Any = (1 << 3) - 1
} Listable;

typedef enum {
    Optional,
    Required
} Choice;

typedef enum {
    Lower = 1 << 0,
    Title = 1 << 1,
    Upper = 1 << 2,
    AnyCase = (1 << 3) - 1
} Case;

typedef enum {
    Before,
    After,
    Both,
    Either,
} Place;

// OPS
static const char SetDifferenceOperator = '-';
static const char SymmetricDifferenceOperator = '%';
static const char IntersectionOperator = '&';

static const char UnionOperator = ',';
static const char HybridOperator = '|';
static const char ConcatOperator = '>';

// WEIGHT
static const char WeightOperator = ':';

// REPETITIONS
static const char LeastOnceQuantifier = '+';
/* "least_zero: null" => no token */
static const char LeastZeroQuantifier = '\0';


// CONSTRAINTS
static const char PositiveConstraintToken = '!';
static const char NegativeConstraintToken = '^';
static const char BoundaryConstraintToken = '$';
static const char SelfConstraintToken = '_';

// COMMENTS
static const char CommentStartToken = '#';
static const char CommentEndToken = ';';
static const Choice CommentEndChoice = Optional;

// IDS
static const char SimpleIDPrefix = '\0';
static const Case SimpleIDCase = AnyCase;
static const char ClassIDPrefix = '@';
static const Case ClassIDCase = AnyCase;

// OPTIONS
static const char OptionalSymbolToken = '?';
static const Place OptionalSymbolPlace = After;
static const Listable OptionalSurround = Parenthesis;

// NORMALS
static const Listable NormalSurround = Brackets;

// CLASS STMTS
static const char ClassToken = '@';
static const char ClassDeclToken = '=';


// ASSERTION STMTS
static const char AssertionBeginToken = '@';
static const Listable AssertionSurround = Brackets;
static const char AssertionSplitter = ',';


// FEATURE STMTS
static const char FeatureStmtPrefix = '!';
// Constraints take from FeatureUse
static const char FeatureNamePrefix = '\0';
static const char FeatureDefaultingPrefix = '*';

// FEATURES
static const char FeaturePrefix = '0';
static const Listable FeatureSurround = Brackets;
static const char FeatureSeperator = ',';

// FEATURE USES
static const char FeatureUseOuterPrefix = '@';
static const Listable FeatureUseSurround = Brackets;
static const char FeatureUseInnerPrefix = '@';

// SYMBOL STATEMENT

static const char SymbolPrefix = '\0';
static const Listable SymbolCase = Brackets;
static const char SymbolAfter = ':';

// ---------------- Write: config -> YAML string ----------------
[[noreturn]]
static int tokens_yaml_write(FILE* out) {
    exit(100);
}

// ---------------- Reader from YAML string: NO parser, strcmp-prefix key match ----------------
// uses mmap
[[noreturn]]
static int tokens_yaml_read_from_string(const char* yaml) {
    exit(101);
}

#endif //WORDMAKE_CONFIG_H
