#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <wave/string.h>
#include <stdlib.h>
#include <string.h>
#include <wave/unicode.h>

struct Wave_String Wave_String_new(const size_t capacity) {
    struct Wave_String s = (struct Wave_String){
        .data.mut_str = NULL,
        .length = 0,
        .utf8_length = 0,
        .capacity = 0
    };

    if (capacity == 0)
        return s;

    s.data.mut_str = (char *)malloc(capacity * sizeof(char));

    if (s.data.mut_str != NULL) {
        s.capacity = capacity;
        s.data.mut_str[0] = '\0';
    }

    return s;
}

struct Wave_String Wave_String_from_and_max_len(const char *const str, const size_t max_len) {
    struct Wave_String s = Wave_String_new(max_len + 1);

    if (s.capacity == 0)
        return s;

    size_t i;
    for (i = 0; i < max_len; i++) {
        s.data.mut_str[i] = str[i];
        if (str[i] == '\0')
            break;

        const size_t ch_len = Wave_Unicode_CharUtf8_get_length_from_byte((uint8_t)str[i]);

        if (ch_len > 0) {
            s.utf8_length++;
        }
    }

    s.data.mut_str[max_len] = '\0';
    s.length = i;
    s.capacity = max_len + 1;

    return s;
}

struct Wave_String Wave_String_from(const char *const str) {
    const size_t str_len = strlen(str);
    struct Wave_String s = Wave_String_from_and_max_len(str, str_len);
    return s;
}

void Wave_String_delete(struct Wave_String *const s) {
    free(s->data.mut_str);
    *s = Wave_String_null;
}

bool Wave_String_reserve(struct Wave_String *const s, const size_t capacity) {

    if (s->capacity == 0) {
        *s = Wave_String_new(capacity);
        return s->capacity != 0;
    }

    if (capacity > s->capacity) {
        char *const reallocated = (char *)realloc(s->data.mut_str, capacity * sizeof(char));
        if (reallocated == NULL)
            return false;
        s->data.mut_str = reallocated;
        s->capacity = capacity;
    }

    return true;
}

bool Wave_String_push(struct Wave_String *const s, const char ch) {
    if (Wave_String_is_view(*s))
        return false;

    if (Wave_String_is_null(*s))
        return false;

    if ((s->length + 1) >= s->capacity) {
        const size_t new_cap = s->capacity * 2;
        if (!Wave_String_reserve(s, new_cap))
            return false;
    }

    s->data.mut_str[s->length] = ch;
    s->length++;
    s->utf8_length++;
    s->data.mut_str[s->length] = '\0';

    return true;
}

bool Wave_String_push_utf8(struct Wave_String *const s, const struct Wave_Unicode_CharUtf8 ch) {
    if (Wave_String_is_view(*s))
        return false;

    if (Wave_String_is_null(*s))
        return false;

    const size_t ch_len = Wave_Unicode_CharUtf8_get_length(ch);

    if (ch_len == 0)
        return false;
    
    if ((s->length + ch_len) >= s->capacity) {
        const size_t new_cap = (s->capacity * 2) + ch_len;
        if (!Wave_String_reserve(s, new_cap))
            return false;
    }

    for (size_t i = 0; i < ch_len; i++) {
        s->data.mut_str[s->length + i] = (char)ch.bytes[i];
    }
    
    s->length += ch_len;
    s->utf8_length++;
    s->data.mut_str[s->length] = '\0';

    return true;
}

bool Wave_String_append(struct Wave_String *const s, const char *const str) {
    if (Wave_String_is_view(*s))
        return false;

    if (Wave_String_is_null(*s)) {
        // mk: should i implicitly create a new string? hmmm... i don't really like implicits
        return false;
    }

    //const size_t str_len = strlen(str);
    size_t str_len = 0;

    for (str_len = 0; str[str_len] != '\0'; str_len++) {
        const size_t ch_len = Wave_Unicode_CharUtf8_get_length_from_byte((uint8_t)str[str_len]);
        if (ch_len > 0) {
            s->utf8_length++;
        }
    }
    
    if ((s->length + str_len + 1) > s->capacity)
        if (!Wave_String_reserve(s, s->length + str_len + 1))
            return false;

    strcpy(s->data.mut_str + s->length, str);
    s->length += str_len;
    s->data.mut_str[s->length] = '\0';

    return true;
}

// mk: length in bytes? no in utf-8
struct Wave_String Wave_String_view_new(const char *const str, const size_t max_utf8_length) {
    struct Wave_String s = (struct Wave_String){
        .data.str = str,
        .length = 0,
        .utf8_length = 0,
        .capacity = 0
    };

    if (str == NULL)
        return s;

    while (str[s.length] != '\0' && s.utf8_length < max_utf8_length) {
        s.length += s.utf8_length += Wave_Unicode_CharUtf8_get_length_from_byte((uint8_t)str[s.length]);
        s.utf8_length++;
    }

    return s;
}

size_t Wave_String_view_expand_utf8(struct Wave_String *const s, const size_t utf8_chars_count) {
    size_t n = 0;

    while (s->data.str[s->length] != '\0' && s->utf8_length < utf8_chars_count) {
        n += s->utf8_length += Wave_Unicode_CharUtf8_get_length_from_byte((uint8_t)s->data.str[s->length]);
        s->utf8_length++;
    }
    s->length += n;
    return n;
}

// mk: implementing this later
/*size_t Wave_String_view_shrink_utf8(struct Wave_String *const s, const size_t utf8_chars_count) {

}*/