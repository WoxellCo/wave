#include "wave/def.h"
#include "wave/lexer.h"
#include <stdint.h>
#include <wave/_debug.h>
#include <stdio.h>

#define WAVE_DEBUG_BEGIN_TEMPLATE   "WAVE %s DEBUG: \n"
#define WAVE_DEBUG_END_TEMPLATE     "WAVE %s DEBUG END\n"

#define WAVE_DEBUG_TOKENIZE_RESULT_META \
    printf("\tn: %zu\n", token_result->n); \
    printf("\terror.type: %u\n", token_result->error.type); \
    printf("\terror.data: ");

void Wave_Debug_string_dump(const struct Wave_String s) {
    printf("WAVE STRING DEBUG (length: %zu, size: %zu)\n", s.length, s.capacity);
    printf("---[\"\"\"%.*s\"\"\"]---", (int)s.length, s.data.str);
    printf("WAVE STRING DEBUG END\n\n");
}



void Wave_Debug_numeric_dump(const struct Wave_Numeric n) {
    //struct Wave_String format = Wave_String_from("NUMBER DEBUG: ");
    printf("NUMERIC DEBUG: \n");

    printf("\tbits (0 = default): %u\n", n.bits);

    if (n.flags & Wave_Numeric_FLOAT) {
        //if (n.bits == 32) {
            //Wave_String_append(&format, "%.3f");
        //}
        if (n.bits == 32) {
            printf("%.3f", n.data.f32);
        } else {
            printf("%.3f", n.data.f64);
        }
    } else if (n.flags == Wave_Numeric_UNSIGNED) {
        //Wave_String_append(&format, "%u");
        switch (n.bits) {
            case 8:
                printf("%u", n.data.u8);
                break;
            case 16:
                printf("%u", n.data.u16);
                break;
            case 32:
                printf("%u", n.data.u32);
                break;
            default:
            case 64:
                #if UINT64_MAX == ULLONG_MAX
                printf("%llu", n.data.u64);
                #else
                printf("%lu", n.data.u64);
                #endif
                break;
        }
    } else {
        switch (n.bits) {
            case 8:
                printf("%d", n.data.i8);
                break;
            case 16:
                printf("%d", n.data.i16);
                break;
            case 32:
                printf("%d", n.data.i32);
                break;
            default:
            case 64:
                #if INT64_MAX == LLONG_MAX
                printf("%lld", n.data.i64);
                #else
                printf("%ld", n.data.i64);
                #endif
                break;
        }
    }

    printf("\n");

    //Wave_String_delete(&format);
}


void Wave_Debug_bitset32_dump(const uint32_t value) {
    printf("WAVE BITSET32 DEBUG: \n");
    printf("\tvalue: %u\n", value);
    printf("\t");

    for (uint8_t i = 0; i < 32; i++) {
        if (i % 8 == 0 && i > 0)
            printf("|");
        printf("%3.1u", (value & (1u << (32 - i - 1))) ? 1 : 0);
    }

    printf("\n\t");

    for (uint8_t i = 0; i < 32; i++) {
        if (i % 8 == 0 && i > 0)
            printf("|");
        printf("%3.1u", i);
    }
    printf("\n");
    printf("WAVE BITSET32 DEBUG END\n\n");
}

void Wave_Debug_token_string_esc(const struct Wave_TokenString_Esc *const token) {
    printf("WAVE TOKEN STRING ESC DEBUG: \n");

    printf("\toutput: \"");
    Wave_Debug_print_string(token->output);
    printf("\"\n");
    printf("\tsequence: \"");
    Wave_Debug_print_string(token->sequence);
    printf("\"\n");
    printf("\tstart_index: \"%zu\"\n", token->start_index);

    printf("WAVE TOKENIZE ESC RESULT DEBUG END\n\n");
}

void Wave_Debug_tokenize_esc_result(const struct Wave_Lexer_TokenizeEscResult *const token_result) {
    printf("WAVE TOKENIZE ESC RESULT DEBUG: \n");

    WAVE_DEBUG_TOKENIZE_RESULT_META

    Wave_Debug_token_string_esc(&token_result->data);
    
    printf("WAVE TOKENIZE ESC RESULT DEBUG END\n\n");
}

void Wave_Debug_tokenize_number_result(const struct Wave_Lexer_TokenizeNumberResult *const token_result) {
    printf(WAVE_DEBUG_BEGIN_TEMPLATE, "TOKENIZE NUMBER RESULT");

    WAVE_DEBUG_TOKENIZE_RESULT_META

    Wave_Debug_numeric_dump(token_result->number);

    printf(WAVE_DEBUG_END_TEMPLATE, "TOKENIZE NUMBER RESULT");
}

void Wave_Debug_tokenize_string_result(const struct Wave_Lexer_TokenizeStringResult *const token_result) {
    printf(WAVE_DEBUG_BEGIN_TEMPLATE, "TOKENIZE STRING RESULT");

    WAVE_DEBUG_TOKENIZE_RESULT_META

    Wave_Debug_string_dump(token_result->string_data);

    printf(WAVE_DEBUG_END_TEMPLATE, "TOKENIZE STRING RESULT");
}

void Wave_Debug_print_string(const struct Wave_String s) {
    printf("%.*s", (int)s.length, s.data.str);
}

void Wave_Debug_println_string(const struct Wave_String s) {
    Wave_Debug_print_string(s);
    putc('\n', stdout);
}
