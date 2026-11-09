#ifndef WAVE__DEBUG_H
#define WAVE__DEBUG_H

#include <stdint.h>
#include <wave.h>
#include <wave/xml.h>

void Wave_Debug_dump_char_loc(const struct Wave_CharLocation loc);
void Wave_Debug_dump_token(const struct Wave_Token t);
void Wave_Debug_dump_token_list(const struct Wave_TokenVector vec);

void Wave_Debug_dump_string(const struct Wave_String s);
void Wave_Debug_tokens_dump(const struct Wave_TokenVector v);
void Wave_Debug_dump_numeric(const struct Wave_Numeric n);
void Wave_Debug_dump_bitset32(const uint32_t value);

void Wave_Debug_print_string(const struct Wave_String s);
void Wave_Debug_println_string(const struct Wave_String s);

void Wave_Debug_token_string_esc(const struct Wave_TokenString_Esc *const token);
void Wave_Debug_tokenize_esc_result(const struct Wave_Lexer_TokenizeEscResult *const token_result);

void Wave_Debug_tokenize_number_result(const struct Wave_Lexer_TokenizeNumberResult *const token_result);

void Wave_Debug_tokenize_string_result(const struct Wave_Lexer_TokenizeStringResult *const token_result);

void Wave_Debug_print_xml_prop(const struct Wave_Xml_Prop *const prop);

void Wave_Debug_compilation_error(const struct Wave_CompilationError error);
void Wave_Debug_dump_token_counters(const struct Wave_TokenCounters *const counters);
void Wave_Debug_dump_dp_params(const struct Wave_Lexer_DelimiterPredicateParams params);

#endif
