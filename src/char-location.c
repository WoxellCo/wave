#include "wave/def.h"
#include "wave/unicode.h"
#include <stddef.h>
#include <wave/char-location.h>

struct Wave_CharLocation_AdvanceResult Wave_CharLocation_advance(
    struct Wave_CharLocation *const loc,
    const char *const source
) {
    struct Wave_CharLocation_AdvanceResult result = {
        .error = Wave_CompilationError_none,
        .ch = {{0}},
        .n = 0
    };
    const struct Wave_Unicode_CharUtf8 ch = Wave_Unicode_CharUtf8_assign((unsigned char *)source);

    if (ch.bytes[0] == 0 && *source != '\0') { // mk: defo an error
        result.error.type = Wave_CompilationErrorType_InvalidUtf8Character;
        return result;
    }

    result.n = Wave_Unicode_CharUtf8_get_length(ch);

    if (ch.bytes[0] == '\n') {
        loc->column = 0;
        loc->utf8_column = 0;
        loc->index++;
        loc->utf8_index++;
        loc->row++;
    } else if (ch.bytes[0] == '\t') {
        loc->column++;
        loc->utf8_column++;
        loc->index++;
        loc->utf8_index++;
    } else if (ch.bytes[0] == '\r') {
        loc->column = 0;
        loc->utf8_column = 0;
        loc->index++;
        loc->utf8_index++;
    } else {
        loc->column += result.n;
        loc->utf8_column++;
        loc->index += result.n;
        loc->utf8_index++;
    }

    return result;
}

size_t Wave_CharLocation_advance_by(
    struct Wave_CharLocation *const loc,
    const char *const source,
    const size_t utf8_chars_count
) {
    size_t n = 0;
    for (size_t i = 0; i < utf8_chars_count; i++) {
        const struct Wave_Unicode_CharUtf8 ch = Wave_Unicode_CharUtf8_assign((unsigned char *)source + n);
        if (ch.bytes[0] == 0 && *source != '\0') {
            break;
        }

        const size_t ch_len = Wave_Unicode_CharUtf8_get_length(ch);
        n += ch_len;

        if (ch.bytes[0] == '\n') {
            loc->column = 0;
            loc->utf8_column = 0;
            loc->index++;
            loc->utf8_index++;
            loc->row++;
        } else if (ch.bytes[0] == '\t') {
            loc->column++;
            loc->utf8_column++;
            loc->index++;
            loc->utf8_index++;
        } else if (ch.bytes[0] == '\r') {
            loc->column = 0;
            loc->utf8_column = 0;
            loc->index++;
            loc->utf8_index++;
        } else {
            loc->column += ch_len;
            loc->utf8_column++;
            loc->index += ch_len;
            loc->utf8_index++;
        }
    }

    return n;
}
