#include <stddef.h>
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
        const EscResult r = Wave_Lexer_tokenize_esc(&loc, t[i], 0, NULL);
        Wave_Debug_tokenize_esc_result(&r);
    }

    return true;
}