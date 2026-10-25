#ifndef WAVE_NUMERIC_H
#define WAVE_NUMERIC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <wave/def.h>

#define Wave_Numeric_UNSIGNED       (1 << 0)
#define Wave_Numeric_FLOAT          (1 << 1)
#define Wave_Numeric_SCIENTIFIC     (1 << 2)

enum Wave_NumericFormat {
    Wave_NumericFormat_Dec = 0,
    Wave_NumericFormat_Hex,
    Wave_NumericFormat_Oct,
    Wave_NumericFormat_Bin,
};

// mk: i can potentially optimize memory usage a lot here
struct Wave_Numeric {
    enum Wave_NumericFormat format;
    uint8_t bits;
    uint8_t flags;
    uint8_t suffix_length;
    union {
        uint8_t     u8 ;
        uint16_t    u16;
        uint32_t    u32;
        uint64_t    u64;
        int8_t      i8 ;
        int16_t     i16;
        int32_t     i32;
        int64_t     i64;
        float       f32;
        double      f64;
    } data;
};

bool Wave_Numeric_string_to_number(
    struct Wave_Numeric *const number,
    const char *const s,
    const size_t length,
    enum Wave_CompilationErrorType *const error_out
);

#endif