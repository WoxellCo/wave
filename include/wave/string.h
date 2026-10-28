#ifndef WAVE_STRING_H
#define WAVE_STRING_H

#include <stddef.h>
#include <stdbool.h>
#include <wave/unicode.h>

struct Wave_String {
    union {
        char *mut_str;
        const char *str;
    } data;
    size_t length;
    size_t utf8_length;
    size_t capacity;
};

#define Wave_String_null ((struct Wave_String){{NULL}, 0, 0, 0})
#define Wave_String_is_null(s) ((s).data.str == NULL && (s).capacity == 0)
#define Wave_String_is_view(s) ((s).data.str != NULL && (s).capacity == 0)

struct Wave_String Wave_String_new(const size_t capacity);
struct Wave_String Wave_String_from(const char *const str);
struct Wave_String Wave_String_from_and_max_len(const char *const str, const size_t max_len);
void Wave_String_delete(struct Wave_String *const s);
bool Wave_String_push(struct Wave_String *const s, const char ch);
bool Wave_String_push_utf8(struct Wave_String *const s, const struct Wave_Unicode_CharUtf8 ch);
bool Wave_String_append(struct Wave_String *const s, const char *const str);

struct Wave_String Wave_String_view_new(const char *const str, const size_t max_utf8_length);
// mk: returns the diff size in bytes
size_t Wave_String_view_expand_utf8(struct Wave_String *const s, const size_t utf8_chars_count);
size_t Wave_String_view_shrink_utf8(struct Wave_String *const s, const size_t utf8_chars_count);

#endif