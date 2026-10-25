#include <wave/token-string.h>
#include <stdlib.h>

struct Wave_TokenString_EscVector Wave_TokenString_EscVector_new(const size_t capacity) {
    struct Wave_TokenString_EscVector vector = (struct Wave_TokenString_EscVector){
        .sequences = NULL,
        .size = 0,
        .capacity = 0,
    };

    if (capacity == 0)
        return vector;

    vector.sequences = (struct Wave_TokenString_Esc *)malloc(capacity * sizeof(struct Wave_TokenString_Esc));
    
    if (vector.sequences != NULL)
        vector.capacity = capacity;

    return vector;
}

bool Wave_TokenString_EscVector_push(struct Wave_TokenString_EscVector *const vector, const struct Wave_TokenString_Esc token) {
    if (vector->size >= vector->capacity) {
        const size_t new_cap = vector->capacity * 2;
        struct Wave_TokenString_Esc *const reallocated = (struct Wave_TokenString_Esc *)realloc(vector->sequences, new_cap * sizeof(struct Wave_TokenString_Esc));
        if (reallocated == NULL)
            return false;
        vector->sequences = reallocated;
        vector->capacity = new_cap;
    }

    vector->sequences[vector->size] = token;
    vector->size++;

    return true;
}


void Wave_TokenString_EscVector_delete(struct Wave_TokenString_EscVector *const vector) {
    free(vector->sequences);
    vector->sequences = NULL;
    vector->size = 0;
    vector->capacity = 0;
}