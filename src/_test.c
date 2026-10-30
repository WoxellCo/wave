#include "wave/lexer.h"
#include <stddef.h>
#include <stdio.h>
#include <wave/_test.h>
#include <wave/_debug.h>
#include <wave.h>

struct Wave_CharLocation Wave_Test_no_loc(void) {
    return (struct Wave_CharLocation){
        .path = NULL,
        .row = 0, .column = 0, .utf8_column = 0,
        .index = 0, .utf8_index = 0
    };
}

bool Wave_Test_escape_sequences(void) {
    typedef struct Wave_Lexer_TokenizeEscResult EscResult;

    const char *t[] = {"\\axyz", "\\u{AAA}", "\\xAA", "\\\\"};

    struct Wave_CharLocation loc = Wave_Test_no_loc();

    for (size_t i = 0; i < 4; i++) {
        printf(">> TEST NO %zu:\n", i + 1);
        EscResult r = Wave_Lexer_tokenize_esc(&loc, t[i], 0, NULL);
        Wave_Debug_tokenize_esc_result(&r);
        Wave_Lexer_clean_token_esc(&r);
    }

    return true;
}

bool Wave_Test_number_literals(void) {
    const char *t[] = {"10.0f32", "16.4", "3", "1e5", "1.1e-4", "70i8", "4f64", "0b100", "0xfF", "0o7"};

    struct Wave_CharLocation loc = Wave_Test_no_loc();

    for (size_t i = 0; i < 10; i++) {
        printf(">> TEST NO %zu:\n", i + 1);
        const struct Wave_Lexer_TokenizeNumberResult r = Wave_Lexer_tokenize_number(&loc, t[i], 0, NULL, NULL);
        Wave_Debug_tokenize_number_result(&r);
    }

    return true;
}

bool Wave_Test_string_literals(void) {
    const char *t[] = {"\"hello world\"", "'bob the sponge'", "\"hello\\nworld\\\"\"", "'hello {\\u{AAA}}'"};

    struct Wave_CharLocation loc = Wave_Test_no_loc();

    for (size_t i = 0; i < 4; i++) {
        printf(">> TEST NO %zu:\n", i + 1);
        struct Wave_Lexer_TokenizeStringResult r = Wave_Lexer_tokenize_string(&loc, t[i], 0, NULL);
        Wave_Debug_tokenize_string_result(&r);
        Wave_Lexer_clean_token_string(&r);
    }

    return true;
}
