#include "wave/_debug.h"
#include "wave/def.h"
#include "wave/lexer.h"
#include "wave/unicode.h"
#include <locale.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wave.h>

int main(void) {
    if (!setlocale(LC_ALL, "")) {
        if (!setlocale(LC_ALL, ".UTF8")) {
            if (!setlocale(LC_ALL, "C.UTF-8") && !setlocale(LC_ALL, "en_US.UTF-8")) {
                fprintf(stderr, "Warning: Could not explicitly set a UTF-8 locale.\n");
            }
        }
    }

    Wave_Unicode_CharUtf8 ch = Wave_Unicode_CharUtf8_get_at(65);
    Wave_Debug_bitset32_dump(ch);
    printf("\'%.*s\'\n", Wave_Unicode_CharUtf8_get_length(ch), (char *)&ch);
    memcpy(&ch, "ÿ", 2);
    printf("\'%.*s\'\n", Wave_Unicode_CharUtf8_get_length(ch), (char *)&ch);
    Wave_Debug_bitset32_dump(ch);

    const unsigned char *p = (const unsigned char *)&ch;
    printf("%02X %02X %02X %02X\n", p[0], p[1], p[2], p[3]);

    printf("%u\n", Wave_Unicode_CharUtf8_get_length(ch));
    printf("%u\n", Wave_Unicode_CharUtf8_get_index(ch));

    char *buffer = NULL;
    FILE *f = fopen("../examples/test.wave", "r");

    size_t length;

    if (f) {
        fseek(f, 0, SEEK_END);
        length = (size_t)ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer) {
            fread(buffer, 1, length, f);
        }
        buffer[length] = '\0';
        fclose(f);
    }

    printf("file: `%s`\n", buffer);

    struct Wave_CharLocation loc = {
        .path = "test.wave",
        .row = 0,
        .column = 0,
        .index = 0
    };

    printf("STRLEN: %zu\n", strlen(buffer));

    Wave_Lexer_tokenize(&loc, buffer, 0, NULL);
}