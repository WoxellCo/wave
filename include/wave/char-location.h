#ifndef WAVE_CHAR_LOCATION_H
#define WAVE_CHAR_LOCATION_H

#include <stddef.h>
#include <wave/def.h>
#include <wave/unicode.h>

// mk: determines which row and column is a specific character, useful for error handling
struct Wave_CharLocation {
    char *path;
    size_t row, column, utf8_column;
    size_t index, utf8_index;
};

// mk: returns how many bytes did the function advance
// mk: no, nvm it would be useful if i returned the utf-8 char instead
//struct Wave_Unicode_CharUtf8 Wave_CharLocation_advance(struct Wave_CharLocation *const loc, const char *const source, const size_t advance_count);
// mk: in addition i will add a result type

struct Wave_CharLocation_AdvanceResult {
    struct Wave_CompilationError error;
    struct Wave_Unicode_CharUtf8 ch;
    size_t n;   // mk: how many bytes this char has
};

struct Wave_CharLocation_AdvanceResult Wave_CharLocation_advance(struct Wave_CharLocation *const loc, const char *const source);

// mk: *this* returns how many bytes did the function advance
size_t Wave_CharLocation_advance_by(struct Wave_CharLocation *const loc, const char *const source, const size_t utf8_chars_count);

#endif
