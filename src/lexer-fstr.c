#include <string.h>
#include <wave/lexer-fstr.h>

struct Wave_Lexer_TokenizeFstrResult Wave_Lexer_tokenize_fstr(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const],
    const struct Wave_TokenCounters *const token_counters
) {
    struct Wave_Lexer_TokenizeFstrResult result = {
        .error = Wave_CompilationError_none,
        .n = 0,
        .flags = 0
    };

    Wave_Lexer_dp_check_derived(coords, token_counters, NULL, 0) {
        // mk: todo: return interrupted by predicate
    }

    const char *src = source + offset;
    const struct Wave_CharLocation_AdvanceResult first_ch_result = Wave_CharLocation_advance(coords, src);
    if (first_ch_result.error.type != Wave_CompilationErrorType_None) {
        result.error = first_ch_result.error;
        return result;
    }
    const struct Wave_Unicode_CharUtf8 first_ch = first_ch_result.ch;

    if (first_ch.bytes[0] != '$') {
        result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
        result.error.info_type = Wave_CompilationErrorInfoType_ExpectedSequence;
        strcpy(result.error.info.expected_sequence, "$");
        return result;
    }

    result.n++;

    src = source + offset + result.n;
    const struct Wave_CharLocation_AdvanceResult bounds_result = Wave_CharLocation_advance(coords, src);
    if (bounds_result.error.type != Wave_CompilationErrorType_None) {
        result.error = bounds_result.error;
        return result;
    }
    const struct Wave_Unicode_CharUtf8 bounds = bounds_result.ch;

    if (bounds.bytes[0] != '"' && bounds.bytes[0] != '\'') {
        result.error.type = Wave_CompilationErrorType_InvalidStringBoundary;
        return result;
    }

    result.n++;
    Wave_Lexer_DelimiterPredicate *extended_predicates = NULL;

    struct Wave_String s = Wave_String_view_new(src + result.n, 0);

    while (true) {
        src = source + offset + result.n;
        
        const struct Wave_CharLocation_AdvanceResult ch_result = Wave_CharLocation_advance(coords, src);
        if (ch_result.error.type != Wave_CompilationErrorType_None) {
            result.error = ch_result.error;
            Wave_String_delete(&s);
            Wave_Lexer_dp_delete_vector(&extended_predicates);
            return result;
        }
        const struct Wave_Unicode_CharUtf8 ch = ch_result.ch;
        const uint8_t ch_first = ch.bytes[0];

        Wave_Lexer_dp_check_derived(coords, NULL, source, coords->index) {
            result.error.type = Wave_CompilationErrorType_UnexpectedBoundary;
            Wave_String_delete(&s);
            result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
            Wave_Lexer_dp_delete_vector(&extended_predicates);
            return result;
        }

        if (ch_first == '\0') {
            result.error.type = Wave_CompilationErrorType_UnexpectedEndOfFile;
            result.error.info.expected_sequence[0] = (char)bounds.bytes[0];
            result.error.info.expected_sequence[1] = '\0';
            result.error.info_type = Wave_CompilationErrorInfoType_ExpectedSequence;
            Wave_String_delete(&s);
            Wave_Lexer_dp_delete_vector(&extended_predicates);
            return result;
        }

        if (ch_first == '\\') {
            struct Wave_Lexer_TokenizeEscResult token_result = Wave_Lexer_tokenize_esc(coords, source, offset + result.n, delimiter_predicates);
            if (token_result.error.type != Wave_CompilationErrorType_None) {
                result.error = token_result.error;
                Wave_String_delete(&s);
                Wave_Lexer_dp_delete_vector(&extended_predicates);
                return result;
            }
            if (token_result.flags & WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE) {
                result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
                result.error.type = Wave_CompilationErrorType_UnexpectedBoundary;
                Wave_String_delete(&s);
                Wave_Lexer_dp_delete_vector(&extended_predicates);
                return result;
            }
            if (Wave_String_is_view(s) && !Wave_String_view_to_owned(&s)) {
                result.error.type = Wave_CompilationErrorType_InvalidAllocation;
                Wave_String_delete(&s);
                Wave_Lexer_dp_delete_vector(&extended_predicates);
                return result;
            }
            if (!Wave_String_append(&s, token_result.data.output.data.str)) {
                result.error.type = Wave_CompilationErrorType_InvalidAllocation;
                Wave_String_delete(&s);
                Wave_Lexer_dp_delete_vector(&extended_predicates);
                return result;
            }
            result.n += token_result.n;
            Wave_Lexer_clean_token_esc(&token_result);
            // mk: remove the `"` or `'` already added before which is one utf-8 char and one byte
            //result.n--; // mk: but not here, becase unlike the charlocation, `result.n` is updated after, but it hits `continue` before
            coords->column--;
            coords->utf8_column--;
            continue;
        } else if (*src == bounds.bytes[0]) {
            result.n++;
            break;
        } else if (*src == '{') {
            Wave_Lexer_dp_check_derived(coords, NULL, source, coords->index + 1) {
                result.error.type = Wave_CompilationErrorType_UnexpectedBoundary;
                Wave_String_delete(&s);
                result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
                return result;
            }

            if (src[1] != '{') {
                if (extended_predicates == NULL) {
                    extended_predicates = Wave_Lexer_dp_new_vector_with_concatenation(delimiter_predicates, Wave_Lexer_dp_array(Wave_Fstr_expr_delimeter_predicate));
                }
                Wave_Lexer_tokenize(coords, source, coords->index, extended_predicates);
            }
        }

        if (Wave_String_is_view(s)) {
            Wave_String_view_expand_utf8(&s, 1);
        } else if (!Wave_String_push_utf8(&s, ch)) {
            result.error.type = Wave_CompilationErrorType_InvalidAllocation;
            Wave_String_delete(&s);
            Wave_Lexer_dp_delete_vector(&extended_predicates);
            return result;
        }

        /*if (flags & escape_seq)
            flags ^= escape_seq;*/

        result.n += ch_result.n;
    }

    //result.string_data = s;
    Wave_Lexer_dp_delete_vector(&extended_predicates);
    return result;
}
