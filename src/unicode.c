#include "wave/_debug.h"
#include <stdint.h>
#include <stdio.h>
#include <wave/unicode.h>

Wave_Unicode_CharUtf8 Wave_Unicode_CharUtf8_get_at(const uint32_t value) {
    if (value > 1114111)
        return 0;
    if (value < 128)
        return value;// << (8 * 3);

    Wave_Unicode_CharUtf8 ch = 0;

    uint8_t bytes[4];
    //*(uint32_t *)bytes = value;             // mk: this is ub bruh

    memcpy(bytes, &value, sizeof(value));

    uint32_t length;
    uint8_t wildcard;
    
    if (value < 2048) {
        length = 2;
        ch = 192u << (8 * (length - 1));
        wildcard = 7;
    } else if (value < 65536) {
        length = 3;
        ch = 224u << (8 * (length - 1));
        wildcard = 15;
    } else {
        length = 4;
        ch = 240u << (8 * (length - 1));
        wildcard = 31;
    }

    for (uint32_t i = 0; i < length - 1; i++) {
        ch |= ((128u | ((value >> (6 * i)) & 63u)) << (8 * i));
    }

    ch |= ((value >> (6 + (length - 1) * 5)) & wildcard) << (6 + (length - 1) * 5);

    return ch;
}

uint8_t Wave_Unicode_CharUtf8_get_length(const Wave_Unicode_CharUtf8 ch) {
    const uint8_t first = *(uint8_t *)&ch;

    // mk: ready, cheap, simple and already unrolled lol
    if (!(first & (1u << 7)))
        return 1;
    
    if (!(first & (1u << 6)))
        return 0;

    if (!(first & (1u << 5)))
        return 2;

    if (!(first & (1u << 4)))
        return 3;

    if (!(first & (1u << 3)))
        return 4;

    return 0;
}

uint32_t Wave_Unicode_CharUtf8_get_index(const Wave_Unicode_CharUtf8 ch) {
    const uint8_t length = Wave_Unicode_CharUtf8_get_length(ch);
    if (length == 0) {
        return 0;
    }
    if (length == 1) {
        return ch;
    }
    uint32_t index = 0;

    uint8_t bytes[4];
    *(uint32_t *)bytes = ch;

    for (uint8_t i = 0; i < length - 1; i++) {
        index |= (((uint32_t)(bytes[length - i - 1]) & 63u) << (6 * i));
    }

    uint8_t wildcard;
    switch (length) {
        case 2:
            wildcard = 31;
            break;
        case 3:
            wildcard = 15;
            break;
        case 4:
            wildcard = 7;
            break;
        default:
            wildcard = 0;
            break;
    }

    index |= ((uint32_t)(bytes[0] & wildcard) << (6 * (length - 1)));

    return index;
}