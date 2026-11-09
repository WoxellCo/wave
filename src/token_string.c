#include "wave/string.h"
#include "wave/token.h"
#include <stddef.h>
#include <wave/def.h>
#include <wave/token_string.h>
#include <stdlib.h>

struct Wave_TokenString_EscVector Wave_TokenString_EscVector_new(struct Wave_Context *const context, const size_t capacity) {
    struct Wave_TokenString_EscVector vector = (struct Wave_TokenString_EscVector){
        .sequences = NULL,
        .size = 0,
        .capacity = 0,
    };

    if (capacity == 0)
        return vector;

    vector.sequences = (struct Wave_TokenString_Esc *)w_malloc(context->allocator.alloc, context->allocator.userdata, capacity * sizeof(struct Wave_TokenString_Esc));
    
    if (vector.sequences != NULL)
        vector.capacity = capacity;

    return vector;
}

bool Wave_TokenString_EscVector_push(struct Wave_Context *const context, struct Wave_TokenString_EscVector *const vector, const struct Wave_TokenString_Esc token) {
    if (vector == NULL)
        return false;
    if (vector->capacity == 0) {
        *vector = Wave_TokenString_EscVector_new(context, 4);
        if (vector->sequences == NULL)
            return false;
    } else if (vector->size >= vector->capacity) {
        const size_t new_cap = vector->capacity * 2;
        struct Wave_TokenString_Esc *const reallocated = (struct Wave_TokenString_Esc *)w_realloc(context->allocator.alloc, context->allocator.userdata, vector->sequences, vector->capacity * sizeof(struct Wave_TokenString_Esc), new_cap * sizeof(struct Wave_TokenString_Esc));
        if (reallocated == NULL)
            return false;
        vector->sequences = reallocated;
        vector->capacity = new_cap;
    }

    vector->sequences[vector->size] = token;
    vector->size++;

    return true;
}


void Wave_TokenString_EscVector_delete(struct Wave_Context *const context, struct Wave_TokenString_EscVector *const vector) {
    if (vector == NULL)
        return;
    if (vector->sequences == NULL || vector->capacity == 0)
        return;


    for (size_t i = 0; i < vector->size; i++) {
        Wave_TokenString_Esc_delete(context, &vector->sequences[i]);
    }

    w_free(context->allocator.alloc, context->allocator.userdata, vector->sequences, vector->capacity * sizeof(struct Wave_TokenString_Esc));
    vector->sequences = NULL;
    vector->size = 0;
    vector->capacity = 0;
}

void Wave_TokenString_delete(struct Wave_Context *const context, struct Wave_TokenString *const token_string) {
    Wave_String_delete(context, &token_string->string_data);
    Wave_TokenString_EscVector_delete(context, &token_string->esc_vector);
}

void Wave_TokenString_Esc_delete(struct Wave_Context *const context, struct Wave_TokenString_Esc *const token_esc) {
    Wave_String_delete(context, &token_esc->output);
    Wave_String_delete(context, &token_esc->sequence);
}
