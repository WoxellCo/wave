#ifndef WAVE_UNICODE_H
#define WAVE_UNICODE_H

#include <stdbool.h>
#include <stdint.h>

struct Wave_Unicode_CharUtf8 {
    uint8_t bytes[4];
};   // mk: the sequence is *not* null-terminated!

// mk: all functions return zero if either result is actual zero or an invalid value is passed
struct Wave_Unicode_CharUtf8 Wave_Unicode_CharUtf8_get_at(const uint32_t value);
uint8_t Wave_Unicode_CharUtf8_get_length(const struct Wave_Unicode_CharUtf8 ch);
uint32_t Wave_Unicode_CharUtf8_get_index(const struct Wave_Unicode_CharUtf8 ch);
uint8_t Wave_Unicode_CharUtf8_get_length_from_byte(const uint8_t byte);
struct Wave_Unicode_CharUtf8 Wave_Unicode_CharUtf8_assign(const unsigned char *const s);
bool Wave_Unicode_CharUtf8_is_multibyte_follower(const uint8_t byte);

#endif