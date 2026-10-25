#ifndef WAVE_TOKEN_STRING_H
#define WAVE_TOKEN_STRING_H

#include <stdint.h>
#include <wave/def.h>

struct Wave_TokenString_EscVector {
    struct Wave_TokenString_Esc *sequences;
    size_t size;
    size_t capacity;
};

// mk: all the information about every single escape sequence
struct Wave_TokenString_Esc {
    struct Wave_String sequence;    // mk: the sequence will be stored in a string, useful for linting and lsp
    struct Wave_String output;      // mk: the output character(s) produced by the sequence
    size_t start_index;             // mk: the start index matches the first byte of the character that replaces
    //uint8_t char_count;           // mk: how many bytes does the output utf-x character have, usually is one, but some cases can have multiple, example `\u{1F98A}` is `🦊`
                                    // mk: nvm i changed my mind.
};

// mk: string token, `string_data` is the actual text a string literal represents with all the escape sequences processed, like all the `\n` are converted to the actual new line character, but they are still stored in the `sequence_vector`!
struct Wave_TokenString {
    struct Wave_String string_data;
    struct Wave_TokenString_EscVector esc_vector;
};

struct Wave_TokenString_EscVector Wave_TokenString_EscVector_new(const size_t capacity);
bool Wave_TokenString_EscVector_push(struct Wave_TokenString_EscVector *const vector, const struct Wave_TokenString_Esc token);
void Wave_TokenString_EscVector_delete(struct Wave_TokenString_EscVector *const vector);

#endif