#ifndef WAVE_LEXER_FSTR_H
#define WAVE_LEXER_FSTR_H

#include <wave/token_fstr.h>
#include <wave/lexer.h>

struct Wave_Lexer_TokenizeFstrResult {
    struct Wave_CompilationError error;
    
    size_t n;
    uint8_t flags;
};

struct Wave_Lexer_TokenizeFstrResult Wave_Lexer_tokenize_fstr(
    struct Wave_Context *const context,
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    #ifndef _MSC_VER
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const],
    #else
    const Wave_Lexer_DelimiterPredicate *const delimiter_predicates,
    #endif
    const struct Wave_TokenCounters *const token_counters
);

#endif
