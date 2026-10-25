#ifndef WAVE_UNICODE_H
#define WAVE_UNICODE_H

#include <stdint.h>

typedef uint32_t Wave_Unicode_CharUtf8;   // mk: the sequence is *not* null-terminated!

// mk: all functions return zero if either result is actual zero or an invalid value is passed
Wave_Unicode_CharUtf8 Wave_Unicode_CharUtf8_get_at(const uint32_t value);
uint8_t Wave_Unicode_CharUtf8_get_length(const Wave_Unicode_CharUtf8 ch);
uint32_t Wave_Unicode_CharUtf8_get_index(const Wave_Unicode_CharUtf8 ch);

#endif