#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <wave/lexer.h>
#include <stdio.h>
#include <stdlib.h>
#include <wave/_debug.h>
#include <wave/lexer-fstr.h>

struct Wave_Lexer_TokenizeResult Wave_Lexer_tokenize(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const]
) {
    struct Wave_Lexer_TokenizeResult result = {
        .error = {
            .type = Wave_CompilationErrorType_None
        },
        .token_vector = Wave_TokenVector_new(128),
        .n = 0,
        .flags = 0
    };

    struct Wave_TokenVector *const tokens = &result.token_vector;

    if (!tokens->capacity) {
        result.error.type = Wave_CompilationErrorType_InvalidAllocation;
        return result;
    }

    if (source == NULL || coords == NULL) {
        result.error.type = Wave_CompilationErrorType_UnexpectedNullPointer;
        Wave_TokenVector_delete(tokens);
        return result;
    }

    struct Wave_TokenCounters token_counters = {0};
    struct Wave_CompilationError compilation_error = {
        .type = Wave_CompilationErrorType_None,
        .info = {0}
    };

    const char *src = source + offset + result.n;
    while (*src != '\0') {
        src = source + offset + result.n;
        const char ch = *src;
        const size_t ch_len = Wave_Unicode_CharUtf8_get_length_from_byte((uint8_t)ch);
        struct Wave_Token token = { // mk: default values that are optimal for those that don't use other tokenizers
            .type = 0,
            .location = *coords,
            .length = 1,
            .data = {0},
        };

        Wave_Lexer_dp_check(Wave_Lexer_dp_params(coords, &token_counters, source, offset + result.n), delimiter_predicates) {
            result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
            return result;
        }

        if (ch_len != 1) { // mk: non-ascii utf-8 characters don't make any sense in the actual code
            result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
            return result;
        }

        if (ch == ' ' || ch == '\t') {
            coords->column++;
            result.n++;
            continue;
        } else if (ch == '\r') {
            coords->column = 0;
            result.n++;
            continue;
        } else if (ch == '\n') {
            coords->column = 0;
            coords->row++;
            result.n++;
            continue;
        } else if (ch == '"' || ch == '\'') {
            const struct Wave_Lexer_TokenizeStringResult string_token_result = Wave_Lexer_tokenize_string(coords, source, result.n, delimiter_predicates);

            if (string_token_result.error.type > 0) {
                // mk: handle error
                compilation_error = string_token_result.error;
                break;
            }

            token = (struct Wave_Token){
                .type = Wave_TokenType_LIT_STR,
                .location = *coords,
                .length = string_token_result.n,
                .data.str = string_token_result.string_data
            };


            result.n += string_token_result.n - 1; // mk: `- 1` because it will be incremented later, it also makes sense because the first character of the other token gets consumed early
            coords->index--; // mk: yeah also decrement this
            coords->column--; // mk: and this?
        } else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') {
            // mk: this can be either a keyword or an identifier
            const struct Wave_Lexer_TokenizeSymbolResult symbol_token_result = Wave_Lexer_tokenize_symbol(coords, source, result.n, delimiter_predicates);

            if (symbol_token_result.error.type > 0) {
                compilation_error = symbol_token_result.error;
                break;
            }

            if (symbol_token_result.flags & WAVE_LEXER_TOKENIZE_SYMBOL_RESULT_KEYWORD) {
                token = (struct Wave_Token){
                    .type = symbol_token_result.data.keyword,
                    .location = *coords,
                    .length = symbol_token_result.n,
                    .data = {0}
                };
            } else {
                token = (struct Wave_Token){
                    .type = Wave_TokenType_ID,
                    .location = *coords,
                    .length = symbol_token_result.n,
                    .data.str = symbol_token_result.data.identifier.id
                };
            }

            result.n += symbol_token_result.n - 1;
            coords->index--;
            coords->column--;
        } else if (ch == '.') {
            // mk: this can be either a member access, spread operator, a range or a floating number
        } else if (ch >= '0' && ch <= '9') {
            printf("pass: `%s`\n\n", src);
            // mk: the number tokenizer should also handle hex (0x), bin (0b), oct (0o), scientific not (1.2e-4) formats
            const struct Wave_Lexer_TokenizeNumberResult number_token_result = Wave_Lexer_tokenize_number(coords, source, result.n, delimiter_predicates, &token_counters);

            if (number_token_result.error.type > 0) {
                compilation_error = number_token_result.error;
                break;
            }

            token = (struct Wave_Token){
                .type = Wave_TokenType_ID,
                .location = *coords,
                .length = number_token_result.n,
                .data.num = number_token_result.number
            };

            result.n += number_token_result.n - 1;
            coords->index--;
            coords->column--;
        } else if ( ch == '=' || //ch == ':' ||     // mk: actually a colon is never used in any kind of operation except assignments, and no, we won't have ternary operators
                    ch == '>' || //ch == '<' ||     // mk: less/greater than operator or xml literal
                    ch == '*' || //ch == '/' ||     // mk: slash can be a comment, regex literal or division
                    ch == '+' || ch == '-' ||
                    ch == '&' || ch == '|' ||
                    ch == '?' || ch == '!' ||
                    ch == '~' || ch == '^' ||
                    ch == '%'
        ) {
            // mk: operators (most of them)
            
        } else if (ch == '/') {
            // mk: comment, division or regex literal

        } else if (ch == '<') {
            // mk: less than, generics? or xml
            
        } else if (ch == '(') {
            token.type = Wave_TokenType_LPAREN;
        } else if (ch == ')') {
            token.type = Wave_TokenType_RPAREN;
        } else if (ch == '[') {
            token.type = Wave_TokenType_LBRACK;
        } else if (ch == ']') {
            token.type = Wave_TokenType_RBRACK;
        } else if (ch == '{') {
            token.type = Wave_TokenType_LBRACE;
        } else if (ch == '}') {
            token.type = Wave_TokenType_RBRACE;
        } else if (ch == ';') {
            token.type = Wave_TokenType_SEMICOLON;
        } else if (ch == ',') {
            token.type = Wave_TokenType_COMMA;
        } else if (ch == ':') {
            // mk: colon or `::`
        } else if (ch == '$') {
            // mk: fstr `$"..."` or `$'...'`
            if (src[1] == '"' || src[1] == '\'') {
                const struct Wave_Lexer_TokenizeFstrResult fstring_token_result = Wave_Lexer_tokenize_fstr(coords, source, coords->index, delimiter_predicates, &token_counters);

                if (fstring_token_result.error.type > 0) {

                }
            } else {
                compilation_error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                break;
            }
        } else {
            // mk: unknown expression/character
            compilation_error.type = Wave_CompilationErrorType_UnexpectedCharacter;
            break;
        }

        Wave_TokenVector_push(&result.token_vector, token);

        coords->column++;
        coords->utf8_column++;
        result.n++;
        coords->index++;
        coords->utf8_index++;
        src = source + offset + result.n;
    }

    if (compilation_error.type != Wave_CompilationErrorType_None) {
        struct Wave_String str = Wave_Error_compilation_error_to_string(coords, source, &compilation_error);
        puts(str.data.mut_str);
        exit(1);
    }

    return result;
}

struct Wave_Lexer_TokenizeStringResult Wave_Lexer_tokenize_string(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const]
) {
    typedef struct Wave_String String;

    struct Wave_Lexer_TokenizeStringResult result = {
        .error = {
            .type = Wave_CompilationErrorType_None
        },
        .string_data = Wave_String_null,
        .n = 0
    };

    if (source == NULL) {
        // mk: handle null exception
        result.error.type = Wave_CompilationErrorType_UnexpectedNullPointer;
        return result;
    }

    const char *src = source + offset;
    //const char bounds = *src;
    const struct Wave_CharLocation_AdvanceResult bounds_result = Wave_CharLocation_advance(coords, src);
    if (bounds_result.error.type != Wave_CompilationErrorType_None) {
        result.error = bounds_result.error;
        return result;
    }
    const struct Wave_Unicode_CharUtf8 bounds = bounds_result.ch;

    if (bounds.bytes[0] != '"' && bounds.bytes[0] != '\'') {
        // mk: handle invalid string bound exception
        result.error.type = Wave_CompilationErrorType_InvalidStringBoundary;
        return result;
    }

    result.n++;

    //String s = Wave_String_new(16);
    String s = Wave_String_view_new(src + result.n, 0);
    //uint8_t flags = 0;
    //const uint8_t escape_mode   = 1 << 0; // mk: should i use a macro or should i trust the compiler optimizing these away?
    //const uint8_t escape_seq    = 1 << 1;

    while (true) {
        src = source + offset + result.n;
        //char ch = *src;
        //coords->column++;
        //coords->index++;
        const struct Wave_CharLocation_AdvanceResult ch_result = Wave_CharLocation_advance(coords, src);
        if (ch_result.error.type != Wave_CompilationErrorType_None) {
            result.error = ch_result.error;
            return result;
        }
        const struct Wave_Unicode_CharUtf8 ch = ch_result.ch;
        const uint8_t ch_first = ch.bytes[0];

        Wave_Lexer_dp_check_derived(coords, NULL, source, coords->index) {
            result.error.type = Wave_CompilationErrorType_UnexpectedBoundary;
            Wave_String_delete(&s);
            result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
            return result;
        }

        if (ch_first == '\0') {
            result.error.type = Wave_CompilationErrorType_UnexpectedEndOfFile;
            result.error.info.expected_sequence[0] = (char)bounds.bytes[0];
            result.error.info.expected_sequence[1] = '\0';
            result.error.info_type = Wave_CompilationErrorInfoType_ExpectedSequence;
            Wave_String_delete(&s);
            return result;
        }

        if (ch_first == '\\') {
            struct Wave_Lexer_TokenizeEscResult token_result = Wave_Lexer_tokenize_esc(coords, source, offset + result.n, delimiter_predicates);
            if (token_result.error.type != Wave_CompilationErrorType_None) {
                result.error = token_result.error;
                Wave_String_delete(&s);
                return result;
            }
            if (token_result.flags & WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE) {
                result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
                result.error.type = Wave_CompilationErrorType_UnexpectedBoundary;
                Wave_String_delete(&s);
                return result;
            }
            if (Wave_String_is_view(s) && !Wave_String_view_to_owned(&s)) {
                result.error.type = Wave_CompilationErrorType_InvalidAllocation;
                Wave_String_delete(&s);
                return result;
            }
            if (!Wave_String_append(&s, token_result.data.output.data.str)) {
                result.error.type = Wave_CompilationErrorType_InvalidAllocation;
                Wave_String_delete(&s);
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
        }

        /*if (flags & escape_mode) {

            

            flags ^= escape_mode;
            flags |= escape_seq;

            coords->index++;
        }

        if (*src == '\\' && !(flags & escape_seq)) {
            flags |= escape_mode;
            result.n++;
            continue;
        } else if (*src == bounds.bytes[0]) {
            result.n++;
            break;
        } else if (*src == '\n') {
            coords->column = 0;
            coords->row++;
        } else if (*src == '\r') {
            coords->column = 0;
            continue;
        }*/

        if (Wave_String_is_view(s)) {
            Wave_String_view_expand_utf8(&s, 1);
        } else if (!Wave_String_push_utf8(&s, ch)) {
            result.error.type = Wave_CompilationErrorType_InvalidAllocation;
            Wave_String_delete(&s);
            return result;
        }

        /*if (flags & escape_seq)
            flags ^= escape_seq;*/

        result.n += ch_result.n;
    }

    result.string_data = s;
    return result;
}

struct Wave_Lexer_TokenizeEscResult Wave_Lexer_tokenize_esc(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const]
) {
    struct Wave_Lexer_TokenizeEscResult result = {
        .error = Wave_CompilationError_none,
        .data = {Wave_String_null, Wave_String_null, 0},
        .n = 0
    };

    /*Wave_Lexer_dp_check(Wave_Lexer_dp_params(coords, NULL, NULL, result.n), delimiter_predicate) {
        return result;
    }*/

    const char *src = source + offset;
    //coords->index++;
    //coords->column++;

    if (*src != '\\') {
        result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
        result.error.info_type = Wave_CompilationErrorInfoType_ExpectedSequence;
        result.error.info.expected_sequence[0] = '\\';
        result.error.info.expected_sequence[1] = '\0';
        return result;
    }

    result.n += Wave_CharLocation_advance_by(coords, src, 1);

    /*result.data.sequence = Wave_String_new(4);
    if (Wave_String_is_null(result.data.sequence)) {
        result.error.type = Wave_CompilationErrorType_InvalidAllocation;
        return result;
    }*/
    result.data.sequence = Wave_String_view_new(src, 1);
  
    result.data.output = Wave_String_new(4);
    if (Wave_String_is_null(result.data.output)) {
        result.error.type = Wave_CompilationErrorType_InvalidAllocation;
        //Wave_String_delete(&result.data.sequence);
        return result;
    }

    /*if (!Wave_String_push(&result.data.sequence, *src)) {
        result.error.type = Wave_CompilationErrorType_InvalidAllocation;
        Wave_String_delete(&result.data.sequence);
        Wave_String_delete(&result.data.output);
        return result;
    }*/

    src = source + offset + result.n;
    char ch = '\0';

    Wave_String_view_expand_utf8(&result.data.sequence, 1);
    /*if (!Wave_String_push(&result.data.sequence, *src)) {
        result.error.type = Wave_CompilationErrorType_InvalidAllocation;
        Wave_String_delete(&result.data.sequence);
        Wave_String_delete(&result.data.output);
        return result;
    }*/
    
    switch (*src) {
        case 'a': ch = '\a'; break;
        case 'b': ch = '\b'; break;
        case 'f': ch = '\f'; break;
        case 'n': ch = '\n'; break;
        case 'r': ch = '\r'; break;
        case 't': ch = '\t'; break;
        case 'v': ch = '\v'; break;
        case '$': ch =  '$'; break;
        case '{': ch =  '{'; break;
        case '\'':ch = '\''; break;
        case '\\':ch = '\\'; break;
        case '"': ch =  '"'; break;
        default:             break;
    }

    if (ch != '\0') {
        if (!Wave_String_push(&result.data.output, ch)) {
            result.error.type = Wave_CompilationErrorType_InvalidAllocation;
            Wave_String_delete(&result.data.sequence);
            Wave_String_delete(&result.data.output);
            return result;
        }
        result.n++;
    } else if (*src == 'x') {
        // mk: 8-bit hex format `\xFF`
        const char *const value_ptr = src + 1;
        for (size_t i = 0; i < 2; i++) {
            result.n += Wave_CharLocation_advance_by(coords, src, 1);
            src = source + offset + result.n;
            Wave_Lexer_dp_check_derived(coords, NULL, source, coords->index) {
                // mk: unfinished escape sequence
                Wave_String_delete(&result.data.sequence);
                Wave_String_delete(&result.data.output);
                result.error.type = Wave_CompilationErrorType_UnexpectedBoundary;
                result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
                return result;
            }

            if ((*src >= '0' && *src <= '9') ||
                (*src >= 'a' && *src <= 'f') ||
                (*src >= 'A' && *src <= 'F')
            ) {
                /*if (!Wave_String_push(&result.data.sequence, *src)) {
                    Wave_String_delete(&result.data.sequence);
                    Wave_String_delete(&result.data.output);
                    result.error.type = Wave_CompilationErrorType_InvalidAllocation;
                    return result;
                }*/
                Wave_String_view_expand_utf8(&result.data.sequence, 1);
            } else {
                Wave_String_delete(&result.data.sequence);
                Wave_String_delete(&result.data.output);
                result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                return result;
            }
        }
        result.n++;
        char *endptr = NULL;
        unsigned long value = strtoul(value_ptr, &endptr, 16);
        const struct Wave_Unicode_CharUtf8 ch_out = Wave_Unicode_CharUtf8_get_at((uint32_t)value);
        if (!Wave_String_push_utf8(&result.data.output, ch_out)) {
            Wave_String_delete(&result.data.sequence);
            Wave_String_delete(&result.data.output);
            result.error.type = Wave_CompilationErrorType_InvalidAllocation;
            return result;
        }
    } else if (*src == 'u') {
        // mk: 24-bit unicode hex format `\u{100000}` (up to six) or `\uFFFF` (only four)
        const char *value_ptr = NULL;
        result.n += Wave_CharLocation_advance_by(coords, src, 1);;
        src = source + offset + result.n;
        size_t value_len = 0;
        if (*src == '{') {
            // mk: 1-6 and closes with `}`
            /*if (!Wave_String_push(&result.data.sequence, '{')) {
                Wave_String_delete(&result.data.sequence);
                Wave_String_delete(&result.data.output);
                result.error.type = Wave_CompilationErrorType_InvalidAllocation;
                return result;
            }*/
            Wave_String_view_expand_utf8(&result.data.sequence, 1);
            value_ptr = src + 1;
            while (true) {
                result.n += Wave_CharLocation_advance_by(coords, src, 1);
                src = source + offset + result.n;
                Wave_Lexer_dp_check_derived(coords, NULL, source, coords->index) {
                    // mk: unfinished escape sequence
                    Wave_String_delete(&result.data.sequence);
                    Wave_String_delete(&result.data.output);
                    result.error.type = Wave_CompilationErrorType_UnexpectedBoundary;
                    result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
                    return result;
                }
                /*if (!Wave_String_push(&result.data.sequence, *src)) {
                    Wave_String_delete(&result.data.sequence);
                    Wave_String_delete(&result.data.output);
                    result.error.type = Wave_CompilationErrorType_InvalidAllocation;
                    return result;
                }*/
                Wave_String_view_expand_utf8(&result.data.sequence, 1);
                if (
                    (*src >= '0' && *src <= '9') ||
                    (*src >= 'a' && *src <= 'f') ||
                    (*src >= 'A' && *src <= 'F')
                ) {
                    value_len++;
                } else if (*src == '}') {
                    break;
                } else {
                    // mk: error: `\u{FF$}
                    Wave_String_delete(&result.data.sequence);
                    Wave_String_delete(&result.data.output);
                    result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                    return result;
                }
            }
        } else {
            // mk: 4 and doesn't close
            value_ptr = src;
            for (size_t i = 0; i < 4; i++) {
                result.n += Wave_CharLocation_advance_by(coords, src, 1);
                src = source + offset + result.n;
                Wave_Lexer_dp_check_derived(coords, NULL, source, coords->index) {
                    // mk: unfinished escape sequence
                    Wave_String_delete(&result.data.sequence);
                    Wave_String_delete(&result.data.output);
                    result.error.type = Wave_CompilationErrorType_UnexpectedBoundary;
                    result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
                    return result;
                }

                if (
                    (*src >= '0' && *src <= '9') ||
                    (*src >= 'a' && *src <= 'f') ||
                    (*src >= 'A' && *src <= 'F')
                ) {
                    if (!Wave_String_push(&result.data.output, *src)) {
                        Wave_String_delete(&result.data.sequence);
                        Wave_String_delete(&result.data.output);
                        result.error.type = Wave_CompilationErrorType_InvalidAllocation;
                        return result;
                    }
                } else {
                    // mk: just no.
                    Wave_String_delete(&result.data.sequence);
                    Wave_String_delete(&result.data.output);
                    result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                    return result;
                }
            }
        }
        result.n++;
        if (value_len < 1 || value_len > 6) {
            Wave_String_delete(&result.data.sequence);
            Wave_String_delete(&result.data.output);
            result.error.type = Wave_CompilationErrorType_InvalidValue;
            return result;
        }
        char *endptr = NULL;
        unsigned long value = strtoul(value_ptr, &endptr, 16);
        const struct Wave_Unicode_CharUtf8 ch_out = Wave_Unicode_CharUtf8_get_at((uint32_t)value);
        if (!Wave_String_push_utf8(&result.data.output, ch_out)) {
            Wave_String_delete(&result.data.sequence);
            Wave_String_delete(&result.data.output);
            result.error.type = Wave_CompilationErrorType_InvalidAllocation;
            return result;
        }
    } else if (*src == '\0') {
        Wave_String_delete(&result.data.sequence);
        Wave_String_delete(&result.data.output);
        result.error.type = Wave_CompilationErrorType_UnexpectedEndOfFile;
        return result;
    } else {
        // mk: unknown sequence, just do nothing, i'll leave it there as placeholder in case i wanna do something in particular in the future
        /*if (!Wave_String_push(&result.data.output, *src)) {
            result.error.type = Wave_CompilationErrorType_InvalidAllocation;
            Wave_String_delete(&result.data.sequence);
            Wave_String_delete(&result.data.output);
            return result;
        }*/
        // mk: nvm just error
        result.error.type = Wave_CompilationErrorType_InvalidSequence;
    }

    return result;
}

#define KW_PUT(kws, type) {(kws), (sizeof(kws) / sizeof(char)) - 1, (type)}

struct Wave_Lexer_TokenizeSymbolResult Wave_Lexer_tokenize_symbol(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const]
) {
    struct Wave_Lexer_TokenizeSymbolResult result = {
        .error = {
            .type = Wave_CompilationErrorType_None
        },
        .data.identifier.id = Wave_String_null,
        .n = 0
    };

    if (source == NULL) {
        result.error.type = Wave_CompilationErrorType_UnexpectedNullPointer;
        return result;
    }

    const char *src = source + offset;
    char ch = *src;

    if (!(ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') {
        // mk: handle invalid identifier start
        result.error.type = Wave_CompilationErrorType_InvalidSymbol;
        return result;
    }

    // mk: alr the first thing im gonna do is check if the symbol is a keyword

    typedef struct { char *kw; size_t len; enum Wave_TokenType type; } KwTuple;
    static const KwTuple keywords[] = {
        KW_PUT  ("if",          Wave_TokenType_IF),
        KW_PUT  ("elif",        Wave_TokenType_ELIF),
        KW_PUT  ("else",        Wave_TokenType_ELSE),
        KW_PUT  ("for",         Wave_TokenType_FOR),
        KW_PUT  ("match",       Wave_TokenType_MATCH),
        KW_PUT  ("switch",      Wave_TokenType_SWITCH),
        KW_PUT  ("case",        Wave_TokenType_CASE),
        KW_PUT  ("default",     Wave_TokenType_DEFAULT),
        KW_PUT  ("break",       Wave_TokenType_BREAK),
        KW_PUT  ("continue",    Wave_TokenType_CONTINUE),
        KW_PUT  ("goto",        Wave_TokenType_GOTO),
        KW_PUT  ("null",        Wave_TokenType_NULL),
        KW_PUT  ("undefined",   Wave_TokenType_UNDEFINED),
        KW_PUT  ("import",      Wave_TokenType_IMPORT),
        KW_PUT  ("export",      Wave_TokenType_EXPORT),
        KW_PUT  ("while",       Wave_TokenType_WHILE),
        KW_PUT  ("do",          Wave_TokenType_DO),
        KW_PUT  ("loop",        Wave_TokenType_LOOP),
        KW_PUT  ("local",       Wave_TokenType_LOCAL),
        KW_PUT  ("global",      Wave_TokenType_GLOBAL),
        KW_PUT  ("u8",          Wave_TokenType_TY_U8),
        KW_PUT  ("u16",         Wave_TokenType_TY_U16),
        KW_PUT  ("u32",         Wave_TokenType_TY_U32),
        KW_PUT  ("u64",         Wave_TokenType_TY_U64),
        KW_PUT  ("u128",        Wave_TokenType_TY_U128),
        KW_PUT  ("i8",          Wave_TokenType_TY_I8),
        KW_PUT  ("i16",         Wave_TokenType_TY_I16),
        KW_PUT  ("i32",         Wave_TokenType_TY_I32),
        KW_PUT  ("i64",         Wave_TokenType_TY_I64),
        KW_PUT  ("i128",        Wave_TokenType_TY_I128),
        KW_PUT  ("f32",         Wave_TokenType_TY_F32),
        KW_PUT  ("f64",         Wave_TokenType_TY_F64),
        KW_PUT  ("v0",          Wave_TokenType_TY_V0),
        KW_PUT  ("bool",        Wave_TokenType_TY_BOOL),
        KW_PUT  ("void",        Wave_TokenType_TY_VOID),
        KW_PUT  ("string",      Wave_TokenType_TY_STRING),
        KW_PUT  ("module",      Wave_TokenType_MODULE),
        KW_PUT  ("class",       Wave_TokenType_CLASS),
        KW_PUT  ("struct",      Wave_TokenType_STRUCT),
        KW_PUT  ("typename",    Wave_TokenType_TYPENAME),
        KW_PUT  ("typeof",      Wave_TokenType_TYPEOF),
        KW_PUT  ("instanceof",  Wave_TokenType_INSTANCEOF),
        KW_PUT  ("sizeof",      Wave_TokenType_SIZEOF),
        KW_PUT  ("public",      Wave_TokenType_PUBLIC),
        KW_PUT  ("pub",         Wave_TokenType_PUB),
        KW_PUT  ("private",     Wave_TokenType_PRIVATE),
        KW_PUT  ("priv",        Wave_TokenType_PRIV),
        KW_PUT  ("protected",   Wave_TokenType_PROTECTED),
        KW_PUT  ("interface",   Wave_TokenType_INTERFACE),
        KW_PUT  ("fn",          Wave_TokenType_FN),
        KW_PUT  ("let",         Wave_TokenType_LET),
        KW_PUT  ("const",       Wave_TokenType_CONST),
        KW_PUT  ("true",        Wave_TokenType_BOOL_TRUE),
        KW_PUT  ("false",       Wave_TokenType_BOOL_FALSE),
        KW_PUT  ("use",         Wave_TokenType_USE),
        KW_PUT  ("namespace",   Wave_TokenType_NAMESPACE),
        KW_PUT  ("return",      Wave_TokenType_RETURN),
        KW_PUT  ("mut",         Wave_TokenType_MUT),
        KW_PUT  ("auto",        Wave_TokenType_AUTO),
        KW_PUT  ("mod",         Wave_TokenType_MOD),
        KW_PUT  ("trait",       Wave_TokenType_TRAIT),
        KW_PUT  ("move",        Wave_TokenType_MOVE),
        KW_PUT  ("async",       Wave_TokenType_ASYNC),
        KW_PUT  ("await",       Wave_TokenType_AWAIT),
        KW_PUT  ("dyn",         Wave_TokenType_DYN),
        KW_PUT  ("enum",        Wave_TokenType_ENUM),
        KW_PUT  ("extern",      Wave_TokenType_EXTERN),
        KW_PUT  ("in",          Wave_TokenType_IN),
        KW_PUT  ("ref",         Wave_TokenType_REF),
        KW_PUT  ("self",        Wave_TokenType_SELF),
        KW_PUT  ("this",        Wave_TokenType_THIS),
        KW_PUT  ("static",      Wave_TokenType_STATIC),
        KW_PUT  ("unsafe",      Wave_TokenType_UNSAFE),
        KW_PUT  ("abstract",    Wave_TokenType_ABSTRACT),
        KW_PUT  ("become",      Wave_TokenType_BECOME),
        KW_PUT  ("box",         Wave_TokenType_BOX),
        KW_PUT  ("final",       Wave_TokenType_FINAL),
        KW_PUT  ("gen",         Wave_TokenType_GEN),
        KW_PUT  ("macro",       Wave_TokenType_MACRO),
        KW_PUT  ("override",    Wave_TokenType_OVERRIDE),
        KW_PUT  ("try",         Wave_TokenType_TRY),
        KW_PUT  ("catch",       Wave_TokenType_CATCH),
        KW_PUT  ("finally",     Wave_TokenType_FINALLY),
        KW_PUT  ("virtual",     Wave_TokenType_VIRTUAL),
        KW_PUT  ("yield",       Wave_TokenType_YIELD),
        KW_PUT  ("new",         Wave_TokenType_NEW),
        KW_PUT  ("delete",      Wave_TokenType_DELETE),
        KW_PUT  ("type",        Wave_TokenType_TYPE),
        KW_PUT  ("as",          Wave_TokenType_AS),
    };

    size_t picked_keyword_idx = SIZE_MAX;    
    for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        const KwTuple keyword = keywords[i];

        if (!strncmp(src, keyword.kw, keyword.len)) {
            const char after = src[keyword.len];
            // mk: check if it's indeed a keyword, like `match` would be a keyword, but `matcha` wouldn't
            if ((after >= 'a' && after >= 'z') ||
                (after >= 'A' && after >= 'Z') ||
                (after >= '0' && after >= '9') ||
                (after == '_'))
                continue;

            picked_keyword_idx = i;
        }
    }

    if (picked_keyword_idx < SIZE_MAX) {
        // mk: it's a keyword and not an identifier
        const KwTuple keyword = keywords[picked_keyword_idx];
        result.data.keyword = keyword.type;
        result.n = keyword.len;
        coords->index += keyword.len;
        coords->column += keyword.len;
        return result;
    }

    while (true) {
        src = source + offset + result.n;
        ch = *src;
        coords->column++;
        coords->index++;

        Wave_Lexer_dp_check_derived(coords, NULL, source, coords->index)
            break;

        // mk: this also automatically covers the null terminator
        if (!((ch >= 'a' && ch <= 'z') ||
              (ch >= 'A' && ch <= 'Z') ||
              (ch >= '0' && ch <= '9') ||
               ch == '_')) {
            break;
        }

        result.n++;
    }

    //result.data.identifier.id = Wave_String_from_and_max_len(source, result.n);
    result.data.identifier.id = Wave_String_view_new(source, result.n);
    Wave_Debug_string_dump(result.data.identifier.id);

    /*if (Wave_String_is_null(result.data.identifier.id)) {
        result.error.type = Wave_CompilationErrorType_InvalidAllocation;
    }*/

    return result;
}

struct Wave_Lexer_TokenizeOperatorResult Wave_Lexer_tokenize_operator(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const]
) {
    struct Wave_Lexer_TokenizeOperatorResult result = {
        .error = Wave_CompilationError_none,
        .operator = 0,
        .n = 1,
        .flags = 0,
    };

    if (source == NULL) {
        result.error.type = Wave_CompilationErrorType_UnexpectedNullPointer;
        return result;
    }

    const char *const src = source + offset;

    uint8_t max_chars = 3;
    if (delimiter_predicates != NULL) {
        for (uint8_t i = 1; i <= 3; i++) {
            Wave_Lexer_dp_check_derived(coords, NULL, source, coords->index + i) {
                max_chars = i;

                result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
                return result;
            }
        }
    }

    switch (src[0]) {
        case '=': //                                                =
            result.operator = Wave_TokenType_ASS;
            if (max_chars >= 2) {
                result.n = 2;
                if (src[1] == '=') { //                             ==
                    result.operator = Wave_TokenType_EQ;
                    if (src[2] == '=' && max_chars >= 3) { //       ===
                        result.n = 3;
                        result.operator = Wave_TokenType_SEQ;
                    }
                } else if (src[1] == '>') { //                      =>
                    result.operator = Wave_TokenType_FARROW;
                }
            }
            break;
        case '!': //                                                !
            result.operator = Wave_TokenType_EMARK;
            if (src[1] == '=' && max_chars >= 2) { //               !=
                result.n = 2;
                result.operator = Wave_TokenType_NEQ;
                if (src[2] == '=' && max_chars >= 3) { //           !==
                    result.n = 3;
                    result.operator = Wave_TokenType_SNEQ;
                }
            }
            break;
        case '<': //                                                <
            result.operator = Wave_TokenType_LT;
            if (max_chars >= 2) {
                result.n = 2;
                if (src[1] == '=') { //                             <=
                    result.operator = Wave_TokenType_LTE;
                } else if (src[2] == '<') { //                      <<
                    result.operator = Wave_TokenType_LSHIFT;
                }
            }
            break;
        case '>': //                                                >
            result.operator = Wave_TokenType_GT;
            if (max_chars >= 2) {
                result.n = 2;
                if (src[1] == '=') { //                             >=
                    result.operator = Wave_TokenType_GTE;
                } else if (src[2] == '>') { //                      >>
                    result.operator = Wave_TokenType_RSHIFT;
                }
            }
            break;
        case '*': //                                                *
            result.operator = Wave_TokenType_MUL;
            
            break;

        default:
            result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
            result.n = 0;
            break;
    }

    return result;
}

struct Wave_Lexer_TokenizeNumberResult Wave_Lexer_tokenize_number(
    struct Wave_CharLocation *const coords,
    const char *const source,
    const size_t offset,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const],
    const struct Wave_TokenCounters *const token_counters
) {
    struct Wave_Lexer_TokenizeNumberResult result = {
        .error = Wave_CompilationError_none,
        .number = {0},
        .n = 0,
        .flags = 0,
    };

    if (source == NULL) {
        result.error.type = Wave_CompilationErrorType_UnexpectedNullPointer;
        return result;
    }

    const char *src = source + offset;
    char ch = *src;

    printf("input: `%s`\n\n", src);

    // mk: this condition only applies to the very first character of the number literal
    if (!((ch >= '0' && ch < '9') || (ch == '.' && src[1] != '.'))) {
        result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
        return result;
    }

    struct Wave_Numeric number = {0};
    number.format = Wave_NumericFormat_Dec;
    size_t floating_position_index = SIZE_MAX;
    uint8_t prefix_length = 0;

    while (true) {
        //coords->column++;
        //coords->index++;
        Wave_Lexer_dp_check_derived(coords, token_counters, source, coords->index) {
            result.flags |= WAVE_LEXER_TOKENIZE_RESULT_INTERRUPTED_BY_PREDICATE;
            return result;
        }

        src = source + offset + result.n;
        //ch = *src;
        const struct Wave_CharLocation_AdvanceResult ch_result = Wave_CharLocation_advance(coords, src);
        if (ch_result.error.type != Wave_CompilationErrorType_None) {
            result.error = ch_result.error;
            return result;
        }
        if (ch_result.n != 1) {
            result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
            return result;
        }
        ch = (char)ch_result.ch.bytes[0];

        if (ch == '.') {
            if (number.flags & Wave_Numeric_FLOAT) {
                // mk: number is already a float, therefore it's not related to the number, it can be a member of the number or range
                break;
            }

            if (src[1] == '.') {
                // mk: that's a range (`..`) or `...`, therefore it's not part of the number
                // mk: but i gotta check if the next dot is part of wave or not first
                if (delimiter_predicates != NULL) {
                    Wave_Lexer_dp_evil_check(Wave_Lexer_dp_params(coords, token_counters, source, offset + result.n + 1), delimiter_predicates) {
                        break;
                    }
                } else {
                    break;
                }
            } else if (src[1] >= '0' && src[1] <= '9') {
                // mk: i've decided that something like `2.` won't be part of a number to allow methods like `2.to_string()`
                Wave_Lexer_dp_check_derived(coords, token_counters, source, offset + result.n + 1) {
                    break;
                }
            } else {
                break;
            }

            if (number.format != Wave_NumericFormat_Dec) {
                break;
            }
            
            if (number.flags & Wave_Numeric_SCIENTIFIC) {
                break;
            }

            floating_position_index = result.n;
            number.flags |= Wave_Numeric_FLOAT;
        } else if (ch == '0' || ch == '1') {
            // mk: that's definetly part of a number
            // mk: keeping separate because it should handle binary format notation
        } else if (ch >= '2' && ch <= '7') {
            if (number.format == Wave_NumericFormat_Bin) {
                result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                return result;
            }
        } else if (ch == '8' || ch == '9') {
            if (number.format == Wave_NumericFormat_Bin ||
                number.format == Wave_NumericFormat_Oct) {
                result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                return result;
            }
        } else if ((ch == 'e' || ch == 'E') && (number.format == Wave_NumericFormat_Dec)) { // mk: dw about `e` being in the first position (which wouldn't be a valid number), it's already handled by the condition before the loop
            // mk: scientific notation
            if (number.flags & Wave_Numeric_SCIENTIFIC) {
                // mk: double `e`, which is not part of the number
                break;
            }
            
            /*if (number.flags != Wave_NumericFormat_Dec) {
                // mk: only decimals can have scientific notation
                break;
            }*/

            // mk: check on the fly if it's valid scientific notation like `3e-4`
            Wave_Lexer_dp_check_derived(coords, token_counters, source, offset + result.n + 1) {
                    // mk: like `1.3e}` => syntax error, will be caught later because it breaks discarding the `e` as well and the `e` will be treated as an identifier which is an invalid token because it's right after a numeric literal
                    break;
            }
            if (!((src[1] == '+' || src[1] == '-') || (src[1] >= '0' && src[1] <= '9'))) {
                // mk: same thing here
                break;
            }

            number.flags |= Wave_Numeric_SCIENTIFIC;
            number.flags |= Wave_Numeric_FLOAT;
            result.n++;
            //coords->index++;
            //coords->column++;
        } else if ((ch == 'a' || (ch >= 'c' && ch <= 'e')) || (ch == 'A' || (ch >= 'C' && ch <= 'F'))
            || (ch == 'f' && (number.format == Wave_NumericFormat_Hex)) // mk: yeah this can either be part of hex or represent `f32` or `f64`
            || ((ch == 'b' || ch == 'B') && (number.format == Wave_NumericFormat_Hex)) // mk: `0b100` or `0x10b`? (disambiguity)
        ) {
            if (number.format != Wave_NumericFormat_Hex) {
                break;
            }
        } else if (result.n == 1 && *(source + offset) == '0') {
            if (ch == 'x' || ch == 'X') {
                number.format = Wave_NumericFormat_Hex;
                prefix_length = 2;
            } else if (ch == 'b' || ch == 'B') {
                number.format = Wave_NumericFormat_Bin;
                prefix_length = 2;
            } else if (ch == 'o' || ch == 'O') {
                number.format = Wave_NumericFormat_Oct;
                prefix_length = 2;
            }
        } else if (ch == 'i' || ch == 'u') {
            // mk: `7i64` or `7u8`
            number.suffix_length = 3;

            if (src[1] == '8') { //                                                     8
                if (src[2] >= '0' && src[2] <= '9') {
                    result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                    return result;
                }
                number.bits = 8;
                number.suffix_length = 2;
            } else if (src[1] == '1' && src[2] == '6') { //                             16
                if (src[3] >= '0' && src[3] <= '9') {
                    result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                    return result;
                }
                number.bits = 16;
            } else if (src[1] == '3' && src[2] == '2') { //                             32
                if (src[3] >= '0' && src[3] <= '9') {
                    result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                    return result;
                }
                number.bits = 32;
            } else if (src[1] == '6' && src[2] == '4') { //                             64
                if (src[3] >= '0' && src[3] <= '9') {
                    result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                    return result;
                }
                number.bits = 64;
            } else { //                                                                 ?
                result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                return result;
            }

            if (number.flags & Wave_Numeric_FLOAT) {
                // mk: if it's a float it can't be an int literal
                result.error.type = Wave_CompilationErrorType_InvalidFloatToIntLiteral;
                return result;
            }

            if (ch == 'u')
                number.flags |= Wave_Numeric_UNSIGNED;

            break;
        } else if (ch == 'f') {
            // mk: `7f32`
            number.flags |= Wave_Numeric_FLOAT;

            if (src[1] == '3' && src[2] == '2') { //                                    32
                if (src[3] >= '0' && src[3] <= '9') {
                    result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                    return result;
                }
                number.bits = 32;
            } else if (src[1] == '6' && src[2] == '4') { //                             64
                if (src[3] >= '0' && src[3] <= '9') {
                    result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                    return result;
                }
                number.bits = 64;
            } else { //                                                                 ?
                result.error.type = Wave_CompilationErrorType_UnexpectedCharacter;
                return result;
            }
            number.suffix_length = 3;

            break;
        } else {
            break;
        }

        result.n++;
    }

    result.n += number.suffix_length;

    printf("result.n (number): %zu\n", result.n);
    printf("\n%zu %d\n\n", floating_position_index, number.suffix_length);

    // mk: finally it's time to convert
    src = number.format == Wave_NumericFormat_Dec ? source + offset : source + offset + 2;
    printf("(%zu, %u)\n", result.n, number.suffix_length);
    printf("`%.*s`\n", (int)result.n - number.suffix_length - prefix_length, src);
    if (!Wave_Numeric_string_to_number(&number, src, result.n - number.suffix_length - prefix_length, &result.error.type)) {
        return result;
    }

    //printf("test: %d\n", t);

    //printf("%llu\n", (unsigned long long)number.data.u64);
    printf(">>>>>>>>>>>>>>>>>>> %.3lf\n", number.data.f64);

    //Wave_Debug_numeric_dump(number);

    return result;
}

bool Wave_Lexer_execute_delimiter_predicates(
    const struct Wave_Lexer_DelimiterPredicateParams params,
    const Wave_Lexer_DelimiterPredicate delimiter_predicates[const]
) {
    if (delimiter_predicates == NULL)
        return false;

    for (size_t i = 0; delimiter_predicates[i] != NULL; i++) {
        if (delimiter_predicates[i](params)) {
            return true;
        }
    }

    return false;
}

Wave_Lexer_DelimiterPredicate *Wave_Lexer_dp_new_vector_with_concatenation(
    const Wave_Lexer_DelimiterPredicate l1[const],
    const Wave_Lexer_DelimiterPredicate l2[const]
) {
    size_t lengths[2] = {0};

    for (lengths[0] = 0; l1[lengths[0]] != NULL; lengths[0]++);
    for (lengths[1] = 0; l2[lengths[1]] != NULL; lengths[1]++);

    Wave_Lexer_DelimiterPredicate *const vector = (Wave_Lexer_DelimiterPredicate *)malloc((lengths[0] + lengths[1] + 1) * sizeof(Wave_Lexer_DelimiterPredicate));
    if (vector == NULL)
        return NULL;

    for (size_t i = 0; i < lengths[0]; i++) {
        vector[i] = l1[i];
    }
    for (size_t i = 0; i < lengths[1]; i++) {
        vector[lengths[0] + i] = l2[i];
    }
    vector[lengths[0] + lengths[1]] = NULL;

    return vector;
}

void Wave_Lexer_dp_delete_vector(Wave_Lexer_DelimiterPredicate **const vector) {
    if (vector == NULL || *vector == NULL)
        return;
    free(*vector);
    *vector = NULL;
}

void Wave_Lexer_clean_token_esc(struct Wave_Lexer_TokenizeEscResult *const token_result) {
    Wave_String_delete(&token_result->data.output);
    Wave_String_delete(&token_result->data.sequence);
}

void Wave_Lexer_clean_token_string(struct Wave_Lexer_TokenizeStringResult *const token_result) {
    Wave_String_delete(&token_result->string_data);
}

void Wave_Lexer_clean_token_symbol(struct Wave_Lexer_TokenizeSymbolResult *const token_result) {
    if (!(token_result->flags & WAVE_LEXER_TOKENIZE_SYMBOL_RESULT_KEYWORD)) {
        Wave_String_delete(&token_result->data.identifier.id);
    }
}
