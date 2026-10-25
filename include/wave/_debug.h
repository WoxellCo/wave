#ifndef WAVE__DEBUG_H
#define WAVE__DEBUG_H

#include <stdint.h>
#include <wave.h>

void Wave_Debug_string_dump(const struct Wave_String s);
void Wave_Debug_tokens_dump(const struct Wave_TokenVector v);
void Wave_Debug_numeric_dump(const struct Wave_Numeric n);
void Wave_Debug_bitset32_dump(const uint32_t value);

#endif