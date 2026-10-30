#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <wave/def.h>
#include <stdlib.h>
#include <wave/token.h>
#include <wave/numeric.h>
#include <wave/unicode.h>

struct Wave_TokenVector Wave_TokenVector_new(const size_t capacity) {
    struct Wave_TokenVector vector = (struct Wave_TokenVector){
        .tokens = NULL,
        .size = 0,
        .capacity = 0,
    };

    if (capacity == 0)
        return vector;

    vector.tokens = (struct Wave_Token *)malloc(capacity * sizeof(struct Wave_Token));
    
    if (vector.tokens != NULL)
        vector.capacity = capacity;

    return vector;
}

bool Wave_TokenVector_push(struct Wave_TokenVector *const vector, const struct Wave_Token token) {
    if (vector->size >= vector->capacity) {
        const size_t new_cap = vector->capacity * 2;
        struct Wave_Token *const reallocated = (struct Wave_Token *)realloc(vector->tokens, new_cap * sizeof(struct Wave_Token));
        if (reallocated == NULL)
            return false;
        vector->tokens = reallocated;
        vector->capacity = new_cap;
    }

    vector->tokens[vector->size] = token;
    vector->size++;

    return true;
}

void Wave_TokenVector_delete(struct Wave_TokenVector *const vector) {
    free(vector->tokens);
    vector->tokens = NULL;
    vector->size = 0;
    vector->capacity = 0;
}

// mk: tried to not use the string conversion from the libc, but i'm too lazy to implement one myself
/*bool Wave_string_to_u8(uint8_t *const value, const char *const s, const size_t length) {
    uint8_t temp = 0;

    for (size_t i = 0; i < length)

    return value;
}*/

#define NM_PUT(bits) \
\
case bits:\
if (number->flags & Wave_Numeric_UNSIGNED) {\
    if (temp.u > UINT##bits##_MAX) {\
        *error_out = Wave_CompilationErrorType_NumberOutOfRange;\
        return false;\
    }\
    number->data.u##bits = (uint##bits##_t)temp.u;\
} else {\
    if (temp.i < INT##bits##_MIN || temp.i > INT##bits##_MAX) {\
        *error_out = Wave_CompilationErrorType_NumberOutOfRange;\
        return false;\
    }\
    number->data.i##bits = (int##bits##_t)temp.i;\
}\
break;

#include <stdio.h>
bool Wave_Numeric_string_to_number(
    struct Wave_Numeric *const number,
    const char *const s,
    const size_t length,
    enum Wave_CompilationErrorType *const error_out
) {
    *error_out = Wave_CompilationErrorType_None;
    int base;

    printf("NNNNNNNNNNNNNNN:::: %.*s\n", (int)length, s);

    switch (number->format) {
        case Wave_NumericFormat_Dec:
            base = 10;
            break;
        case Wave_NumericFormat_Bin:
            base = 2;
            break;
        case Wave_NumericFormat_Oct:
            base = 8;
            break;
        case Wave_NumericFormat_Hex:
            base = 16;
            break;
        default:
            return false;
    }

    char *const buffer = (char *)malloc((length + 1) * sizeof(char));
    if (buffer == NULL) {
        *error_out = Wave_CompilationErrorType_InvalidAllocation;
        return false;
    }

    strncpy(buffer, s, length);
    buffer[length] = '\0';

    //puts(buffer);

    if (number->flags & Wave_Numeric_FLOAT) {
        // mk: floats are decimal only
        char *endptr = NULL;
        const double temp = strtod(buffer, &endptr);

        if (number->bits == 32) {
            number->data.f32 = (float)temp;
        } else {
            number->data.f64 = temp;
        }
    } else /*if (number->flags & Wave_Numeric_UNSIGNED)*/ {
        // mk: this is definetly an integer
        union {
            unsigned long long int u;
            long long int i;
        } temp;
        char *endptr = NULL;

        if (number->flags & Wave_Numeric_UNSIGNED) {
            temp.u = strtoull(buffer, &endptr, base);
        } else {
            temp.i = strtoll(buffer, &endptr, base);
        }

        switch (number->bits) {
            NM_PUT(8)
            NM_PUT(16)
            NM_PUT(32)
            default:
            NM_PUT(64)
        }
    } //else {
        // mk: signed integer
        // mk: actually no.
    //}

    free(buffer);

    return true;
}
