#ifndef WAVE_LEXER_H
#define WAVE_LEXER_H

#include "wave/string.h"
#include "wave/token-string.h"
#include <stddef.h>
#include <stdint.h>
#include <wave/def.h>
#include <wave/token.h>

#define WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE     (1 << 0)

#define WAVE_LEXER_TOKENIZE_SYMBOL_RESULT_KEYWORD               (1 << 1)

struct Wave_Lexer_TokenizeResult {
    struct Wave_CompilationError error;
    struct Wave_TokenVector token_vector;
    size_t n;                       // mk: `n` is the number of characters of the source code consumed by the tokenizer
    uint8_t flags;                  // mk: in this case it signals if the cycle was interrputed by the predicate
};

struct Wave_Lexer_DelimiterPredicateParams {
    const struct Wave_CharLocation *const loc;              // mk: information of the char positions passed
    const struct Wave_TokenCounters *const token_counters;  // mk: the predicate needs to know if it can actually terminate the script based on the counts
    const char *const source;                               // mk: the source the predicate has to check (source + n)
    const size_t n;                                         // mk: number of processed characters (excluding delimiter)
};

typedef bool(*Wave_Lexer_DelimiterPredicate)(
    const struct Wave_Lexer_DelimiterPredicateParams params
);

// mk: the cool thing about the following solution for delimiter predicates is that, the `NULL` check is done automatically

// mk: dp => delimiter predicate(s)
#define Wave_Lexer_dp_params(loc_, counters_, src_, n_) ((struct Wave_Lexer_DelimiterPredicateParams){.loc = (loc_), .token_counters = (counters_), .source = (src_), .n = (n_)})
#define Wave_Lexer_dp_array(...) ((Wave_Lexer_DelimiterPredicate[]){__VA_ARGS__, NULL})
#define Wave_Lexer_dp_check(params, predicates) if (Wave_Lexer_execute_delimiter_predicates((params), (predicates)))
#define Wave_Lexer_dp_evil_check(params, predicates) if (!Wave_Lexer_execute_delimiter_predicates((params), (predicates)))
#define Wave_Lexer_dp_check_derived(loc_, counters_, src_, n_) Wave_Lexer_dp_check(Wave_Lexer_dp_params((loc_), (counters_), (src_), (n_)), delimiter_predicates)

Wave_Lexer_DelimiterPredicate *Wave_Lexer_dp_new_vector_with_concatenation(
    const Wave_Lexer_DelimiterPredicate l1[const],
    const Wave_Lexer_DelimiterPredicate l2[const]
);

void Wave_Lexer_dp_delete_vector(Wave_Lexer_DelimiterPredicate **const vector);

// mk: null-terminated array of delimiter predicates
bool Wave_Lexer_execute_delimiter_predicates(
    const struct Wave_Lexer_DelimiterPredicateParams params,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const]
);

struct Wave_Lexer_TokenizeResult Wave_Lexer_tokenize(
    struct Wave_CharLocation *const coords,                         // mk: initial coords, it mutates during the execution
    const char *const source,                                       // mk: source code to tokenize
    const size_t offset,                                            // mk: offset, if the code begins at character 12 and offset is 2, the tokenizer starta at characters 14
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const] // mk: the predicate controls if the token should keep consuming the characters or not (returns a true if it should stop), it can be `NULL` meaning there will be no checks and the tokenizer will consume until EOF
);

struct Wave_Lexer_TokenizeStringResult {
    struct Wave_CompilationError error;
    struct Wave_String string_data;
    size_t n;                       // mk: `n` is the number of characters of the source code consumed by the tokenizer
    uint8_t flags;                  // mk: in this case it signals if the cycle was interrputed by the predicate
};

struct Wave_Lexer_TokenizeStringResult Wave_Lexer_tokenize_string(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const]
);

struct Wave_Lexer_TokenizeSymbolResult {
    struct Wave_CompilationError error;
    union {
        struct Wave_Identifier identifier;
        enum Wave_TokenType keyword;
    } data;
    size_t n;
    uint8_t flags;
};

struct Wave_Lexer_TokenizeSymbolResult Wave_Lexer_tokenize_symbol(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const]
);

struct Wave_Lexer_TokenizeOperatorResult {
    struct Wave_CompilationError error;
    enum Wave_TokenType operator;
    size_t n;
    uint8_t flags;
};

struct Wave_Lexer_TokenizeOperatorResult Wave_Lexer_tokenize_operator(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const] // mk: important: i should add the counters so the predicate can evaluate better
);

struct Wave_Lexer_TokenizeNumberResult {
    struct Wave_CompilationError error;
    struct Wave_Numeric number;
    size_t n;
    uint8_t flags;
};

struct Wave_Lexer_TokenizeNumberResult Wave_Lexer_tokenize_number(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const],
    const struct Wave_TokenCounters *const token_counters
);

struct Wave_Lexer_TokenizeEscResult {
    struct Wave_CompilationError error;
    struct Wave_TokenString_Esc data;
    size_t n;
    uint8_t flags;
};

struct Wave_Lexer_TokenizeEscResult Wave_Lexer_tokenize_esc(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const]
);

// mk: easy clean up result token helper functions, they completely free heap allocated memory
void Wave_Lexer_clean_token_esc(struct Wave_Lexer_TokenizeEscResult *const token_result);
void Wave_Lexer_clean_token_string(struct Wave_Lexer_TokenizeStringResult *const token_result);
void Wave_Lexer_clean_token_symbol(struct Wave_Lexer_TokenizeSymbolResult *const token_result);

#endif
