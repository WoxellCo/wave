#include "wave/def.h"
#include <stdint.h>
#include <wave/_debug.h>
#include <stdio.h>

void Wave_Debug_string_dump(const struct Wave_String s) {
    printf("WAVE STRING DEBUG (length: %zu, size: %zu)\n", s.length, s.capacity);
    puts(s.str);
    printf("WAVE STRING DEBUG END\n\n");
}



void Wave_Debug_numeric_dump(const struct Wave_Numeric n) {
    //struct Wave_String format = Wave_String_from("NUMBER DEBUG: ");
    printf("NUMERIC DEBUG: \n");

    printf("\tbits (0 = default): %u\n", n.bits);

    if (n.flags & Wave_Numeric_FLOAT) {
        //if (n.bits == 32) {
            //Wave_String_append(&format, "%.3f");
        //}
        if (n.bits == 32) {
            printf("%.3f", n.data.f32);
        } else {
            printf("%.3f", n.data.f64);
        }
    } else if (n.flags == Wave_Numeric_UNSIGNED) {
        //Wave_String_append(&format, "%u");
        switch (n.bits) {
            case 8:
                printf("%u", n.data.u8);
                break;
            case 16:
                printf("%u", n.data.u16);
                break;
            case 32:
                printf("%u", n.data.u32);
                break;
            default:
            case 64:
                printf("%lu", n.data.u64);
                break;
        }
    } else {
        switch (n.bits) {
            case 8:
                printf("%d", n.data.i8);
                break;
            case 16:
                printf("%d", n.data.i16);
                break;
            case 32:
                printf("%d", n.data.i32);
                break;
            default:
            case 64:
                printf("%ld", n.data.i64);
                break;
        }
    }

    printf("\n");

    //Wave_String_delete(&format);
}


void Wave_Debug_bitset32_dump(const uint32_t value) {
    printf("WAVE BITSET32 DEBUG: \n");
    printf("\tvalue: %u\n", value);
    printf("\t");

    for (uint8_t i = 0; i < 32; i++) {
        if (i % 8 == 0 && i > 0)
            printf("|");
        printf("%3.1u", (value & (1u << (32 - i - 1))) ? 1 : 0);
    }

    printf("\n\t");

    for (uint8_t i = 0; i < 32; i++) {
        if (i % 8 == 0 && i > 0)
            printf("|");
        printf("%3.1u", i);
    }
    printf("\n");
    printf("WAVE BITSET32 DEBUG END\n\n");
}