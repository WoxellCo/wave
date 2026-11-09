#include <wave/char_location.h>
#include <wave/context.h>
#include <wave/lexer.h>
#include <wave/token.h>
#include <wave/xml.h>
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

bool Wave_Test_escape_sequences(struct Wave_Context *const context) {
    typedef struct Wave_Lexer_TokenizeEscResult EscResult;

    const char *t[] = {"\\axyz", "\\u{AAA}", "\\xAA", "\\\\"};

    struct Wave_CharLocation loc = Wave_Test_no_loc();

    for (size_t i = 0; i < 4; i++) {
        printf(">> TEST NO %zu:\n", i + 1);
        EscResult r = Wave_Lexer_tokenize_esc(context, &loc, t[i], 0, NULL);
        Wave_Debug_tokenize_esc_result(&r);
        Wave_Lexer_clean_token_esc(context, &r);
    }

    return true;
}

bool Wave_Test_number_literals(struct Wave_Context *const context) {
    const char *t[] = {"10.0f32", "16.4", "3", "1e5", "1.1e-4", "70i8", "4f64", "0b100", "0xfF", "0o7"};

    struct Wave_CharLocation loc = Wave_Test_no_loc();

    for (size_t i = 0; i < 10; i++) {
        printf(">> TEST NO %zu:\n", i + 1);
        const struct Wave_Lexer_TokenizeNumberResult r = Wave_Lexer_tokenize_number(context, &loc, t[i], 0, NULL, NULL);
        Wave_Debug_tokenize_number_result(&r);
    }

    return true;
}

bool Wave_Test_string_literals(struct Wave_Context *const context) {
    const char *t[] = {"\"hello world\"", "'bob the sponge'", "\"hello\\nworld\\\"\"", "'hello {\\u{AAA}}'"};

    struct Wave_CharLocation loc = Wave_Test_no_loc();

    for (size_t i = 3; i < 4; i++) {
        printf(">> TEST NO %zu:\n", i + 1);
        struct Wave_Lexer_TokenizeStringResult r = Wave_Lexer_tokenize_string(context, &loc, t[i], 0, NULL);
        Wave_Debug_tokenize_string_result(&r);
        Wave_Lexer_clean_token_string(context, &r);
    }

    return true;
}

bool Wave_Test_symbols(struct Wave_Context *const context) {
    const char *t[] = {"let", "const", "match", "matcha"};

    struct Wave_CharLocation loc = Wave_Test_no_loc();

    for (size_t i = 0; i < 4; i++) {
        printf(">> TEST NO %zu:\n", i + 1);
        struct Wave_Lexer_TokenizeSymbolResult r = Wave_Lexer_tokenize_symbol(&loc, t[i], 0, NULL);
        struct Wave_Token token = {
            .type = 0,
            .location = loc,
            .length = 1,
            .data = {0},
            .category = Wave_TokenCat_None
        };
        if (r.flags & WAVE_LEXER_TOKENIZE_SYMBOL_RESULT_KEYWORD) {
            token = (struct Wave_Token){
                .type = r.data.keyword,
                .location = token.location,
                .length = r.n,
                .data = {0},
                .category = Wave_TokenCat_Keyword
            };
        } else {
            token = (struct Wave_Token){
                .type = Wave_TokenType_ID,
                .location = token.location,
                .length = r.n,
                .data.text = r.data.identifier.id
            };
        }
        Wave_Debug_dump_token(token);
        Wave_Token_delete(context, &token);
    }

    return true;
}

bool Wave_Test_Xml_prop(struct Wave_Context *const context) {
    const char *t[] = {
        "hello=\"'world&quot;\"",
        "key='\"value&quot;'",
        "k1 =\"v1\"",
        "k2= 'v2'",
        "t1=\"{&#050;}\"",
        "t2=\"{&#x050;}\"",
        "t3=\"&amp;o&#x050;&copy;&#x051;&#x052;\"",
        "x1={2 + 2}",
        "x2={{\"x\"}}",
    };

    struct Wave_CharLocation loc = Wave_Test_no_loc();

    for (size_t i = 0; i < 9; i++) {
        loc = Wave_Test_no_loc();
        struct Wave_Xml_Prop prop;
        const struct Wave_Xml_LexResult r = Wave_Xml_Prop_lex(context, &prop, &loc, t[i], 0, NULL);
        Wave_Debug_compilation_error(r.error);
        Wave_Debug_print_xml_prop(&prop);
        putc('\n', stdout);
        Wave_Xml_Prop_delete(context, &prop);
    }

    return true;
}

bool Wave_Test_Xml(struct Wave_Context *const context) {
    const char *t[] = {
        "<>\n\
            <hello prop=\"value\"><div/></hello>\n\
        </>"
    };

    (void)context;
    (void)t;

    return true;
}
