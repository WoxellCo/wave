#include "wave/def.h"
#include <string.h>
#include <wave/lexer-fstr.h>

struct Wave_Lexer_TokenizeFstrResult Wave_Lexer_tokenize_fstr(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicate,
    const struct Wave_TokenCounters *const token_counters
) {
    if (delimiter_predicate != NULL)
        delimiter_predicate(coords, token_counters, NULL, 0);
    struct Wave_Lexer_TokenizeFstrResult result = {
        .error = Wave_CompilationError_none,
        .n = 0,
        .flags = 0
    };

    const char *src = source + offset;
    const char bounds = src[1];

    coords->index++;
    coords->column++;

    if (src[0] != '$') {
        result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
        result.error.info_type = Wave_CompilationErrorInfoType_ExpectedSequence;
        strcpy(result.error.info.expected_sequence, "$");
        return result;
    }

    result.n++;
    coords->index++;
    coords->column++;

    if (bounds != '"' && bounds != '\'') {
        result.error.type = Wave_CompilationErrorType_InvalidStringBoundary;
        return result;
    }

    result.n++;

    struct Wave_String s = Wave_String_new(16);
    uint8_t flags = 0;
    const uint8_t escape_mode   = 1 << 0; // mk: should i use a macro or should i trust the compiler optimizing these away?
    const uint8_t escape_seq    = 1 << 1;

    while (true) {
        src = source + offset + result.n;
        char ch = *src;
        coords->index++;
        coords->column++;

        if (delimiter_predicate != NULL) {
            if (delimiter_predicate(coords, NULL, source, coords->index)) {
                result.error.type = Wave_CompilationErrorType_UnexpectedBoundary;
                Wave_String_delete(&s);
                result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
                return result;
            }
        }

        if (ch == '\0') {
            result.error.type = Wave_CompilationErrorType_UnexpectedEndOfFile;
            result.error.info.expected_sequence[0] = bounds;
            result.error.info.expected_sequence[1] = '\0';
            result.error.info_type = Wave_CompilationErrorInfoType_ExpectedSequence;
            Wave_String_delete(&s);
            return result;
        }

        if (flags & escape_mode) {

            switch (ch) {
                case 'a': ch = '\a'; break;
                case 'b': ch = '\b'; break;
                case 'f': ch = '\f'; break;
                case 'n': ch = '\n'; break;
                case 'r': ch = '\r'; break;
                case 't': ch = '\t'; break;
                case 'v': ch = '\v'; break;
                default:             break;
            }

            flags ^= escape_mode;
            flags |= escape_seq;

            coords->index++;
        }

        if (ch == '\\' && !(flags & escape_seq)) {
            flags |= escape_mode;
            result.n++;
            continue;
        } else if (ch == bounds) {
            result.n++;
            break;
        } else if (ch == '\n') {
            coords->column = 0;
            coords->row++;
        } else if (ch == '\r') {
            coords->column = 0;
            continue;
        } else if (ch == '{') {
            
        }

        if (!Wave_String_push(&s, ch)) {
            result.error.type = Wave_CompilationErrorType_InvalidAllocation;
            Wave_String_delete(&s);
            return result;
        }

        if (flags & escape_seq)
            flags ^= escape_seq;

        result.n++;
    }

    //result.string_data = s;
    return result;
}
