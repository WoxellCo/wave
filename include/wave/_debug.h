#ifndef WAVE__DEBUG_H
#define WAVE__DEBUG_H

#include <stdint.h>
#include <wave.h>

void Wave_Debug_string_dump(const struct Wave_String s);
void Wave_Debug_tokens_dump(const struct Wave_TokenVector v);
void Wave_Debug_numeric_dump(const struct Wave_Numeric n);
void Wave_Debug_bitset32_dump(const uint32_t value);

void Wave_Debug_print_string(const struct Wave_String s);
void Wave_Debug_println_string(const struct Wave_String s);

void Wave_Debug_token_string_esc(const struct Wave_TokenString_Esc *const token);
void Wave_Debug_tokenize_esc_result(const struct Wave_Lexer_TokenizeEscResult *const token_result);

void Wave_Debug_tokenize_number_result(const struct Wave_Lexer_TokenizeNumberResult *const token_result);

void Wave_Debug_tokenize_string_result(const struct Wave_Lexer_TokenizeStringResult *const token_result);

#endif
