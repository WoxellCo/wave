#include <complex.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <wave/unicode.h>

struct Wave_Unicode_CharUtf8 Wave_Unicode_CharUtf8_get_at(const uint32_t value) {
    struct Wave_Unicode_CharUtf8 ch = {{0}};

    if (value > 1114111)
        return ch;
    if (value < 128) {
        ch.bytes[0] = value & 127;
        return ch;
    }

    uint8_t bytes[4];
    //*(uint32_t *)bytes = value;             // mk: this is ub bruh

    memcpy(bytes, &value, sizeof(value));

    uint32_t length;
    uint8_t wildcard;
    
    if (value < 2048) {
        length = 2;
        ch.bytes[0] = 192u;
        wildcard = 7;
    } else if (value < 65536) {
        length = 3;
        ch.bytes[0] = 224u;
        wildcard = 15;
    } else {
        length = 4;
        ch.bytes[0] = 240u;
        wildcard = 31;
    }

    for (uint32_t i = 0; i < length - 1; i++) {
        ch.bytes[length - i - 1] = 128u | ((value >> (6 * i)) & 63u);
    }

    ch.bytes[0] |= (uint8_t)(value >> (6 * (length - 1))) & wildcard;

    return ch;
}

uint8_t Wave_Unicode_CharUtf8_get_length(const struct Wave_Unicode_CharUtf8 ch) {
    return Wave_Unicode_CharUtf8_get_length_from_byte(ch.bytes[0]);
}

uint8_t Wave_Unicode_CharUtf8_get_length_from_byte(const uint8_t byte) {
    // mk: ready, cheap, simple and already unrolled lol
    if (!(byte & (1u << 7)))
        return 1;
    
    if (!(byte & (1u << 6)))
        return 0;

    if (!(byte & (1u << 5)))
        return 2;

    if (!(byte & (1u << 4)))
        return 3;

    if (!(byte & (1u << 3)))
        return 4;

    return 0;
}

uint32_t Wave_Unicode_CharUtf8_get_index(const struct Wave_Unicode_CharUtf8 ch) {
    const uint8_t length = Wave_Unicode_CharUtf8_get_length(ch);
    if (length == 0) {
        return 0;
    }
    if (length == 1) {
        return ch.bytes[0];
    }
    uint32_t index = 0;

    for (uint8_t i = 0; i < length - 1; i++) {
        index |= (((uint32_t)(ch.bytes[length - i - 1]) & 63u) << (6 * i));
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

    index |= ((uint32_t)(ch.bytes[0] & wildcard) << (6 * (length - 1)));

    return index;
}

struct Wave_Unicode_CharUtf8 Wave_Unicode_CharUtf8_assign(const unsigned char *const s) {
    struct Wave_Unicode_CharUtf8 ch = {{0}};
    //const size_t len = strlen(s);

    const size_t expected_len = Wave_Unicode_CharUtf8_get_length_from_byte(*s);

    if (expected_len == 0) {
        return ch;
    }

    size_t len = 0;
    for (len = 1; len < 4; len++) {
        if (!Wave_Unicode_CharUtf8_is_multibyte_follower(s[len])) {
            break;
        }
    }

    if (len != expected_len) {
        return ch;
    }

    memcpy(ch.bytes, s, len);

    return ch;
}

bool Wave_Unicode_CharUtf8_is_multibyte_follower(const uint8_t byte) {
    return ((byte & 192u) == (1u << 7));
}
