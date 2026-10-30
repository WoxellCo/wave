#ifndef WAVE_LEXER_FSTR_H
#define WAVE_LEXER_FSTR_H

#include <wave/token-fstr.h>
#include <wave/lexer.h>

struct Wave_Lexer_TokenizeFstrResult {
    struct Wave_CompilationError error;
    
    size_t n;
    uint8_t flags;
};

struct Wave_Lexer_TokenizeFstrResult Wave_Lexer_tokenize_fstr(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const],
    const struct Wave_TokenCounters *const token_counters
);

#endif
