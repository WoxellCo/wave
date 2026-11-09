#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <wave/def.h>
#include <stdlib.h>
#include <wave/token.h>
#include <wave/numeric.h>
#include <wave/unicode.h>

#include <wave/int-target.h>

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
    struct Wave_Context *const context,
    struct Wave_Numeric *const number,
    const char *const s,
    const size_t length,
    enum Wave_CompilationErrorType *const error_out
) {
    *error_out = Wave_CompilationErrorType_None;
    int base;

    //printf("NNNNNNNNNNNNNNN:::: %.*s\n", (int)length, s);

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

    char *const buffer = (char *)w_malloc(context->allocator.alloc, context->allocator.userdata, (length + 1) * sizeof(char));
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

    w_free(context->allocator.alloc, context->allocator.userdata, buffer, (length + 1) * sizeof(char));

    return true;
}

// mk: temporary function used to help with development
enum Wave_CompilationErrorType Wave_debugger_log(const char *const file, const size_t line) {
    printf("%s:%zu: wave dbg triggered here!\n", file, line);
    return Wave_CompilationErrorType_UnexpectedCharacter;//Wave_CompilationErrorType_UnexpectedCharacter_aa;
}
