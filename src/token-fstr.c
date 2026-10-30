#include "wave/token.h"
#include <wave/token-fstr.h>
#include <stdlib.h>

//bool Wave_Fstr_expr_delimeter_predicate(const Wave_Lexer_DelimiterPredicate user_delimiter_predicate);
bool Wave_Fstr_expr_delimeter_predicate(
    const struct Wave_CharLocation *const coords,
    const struct Wave_TokenCounters *const token_counters,
    const char *const source,
    const size_t n,
    const struct Wave_Lexer_EncapsulatedDelimiterPredicate *const chain
)
{
    if (chain != NULL && chain->delimiter_predicate(coords, token_counters, source, n, NULL))
        return true;
    
    return (source[n] == ':' || source[n] == '}') && Wave_Token_do_counters_match(token_counters);
}

struct Wave_TokenFstr_EscOrExprVector Wave_TokenString_EscOrExprVector_new(const size_t capacity) {
    struct Wave_TokenFstr_EscOrExprVector vector = (struct Wave_TokenFstr_EscOrExprVector){
        .elements = NULL,
        .size = 0,
        .capacity = 0,
    };

    if (capacity == 0)
        return vector;

    vector.elements = (struct Wave_TokenFstr_Element *)malloc(capacity * sizeof(struct Wave_TokenFstr_Element));
    
    if (vector.elements != NULL)
        vector.capacity = capacity;

    return vector;
}

bool Wave_TokenString_EscOrExprVector_push(struct Wave_TokenFstr_EscOrExprVector *const vector, const struct Wave_TokenFstr_Element token) {
    if (vector->size >= vector->capacity) {
        const size_t new_cap = vector->capacity * 2;
        struct Wave_TokenFstr_Element *const reallocated = (struct Wave_TokenFstr_Element *)realloc(vector->elements, new_cap * sizeof(struct Wave_TokenFstr_Element));
        if (reallocated == NULL)
            return false;
        vector->elements = reallocated;
        vector->capacity = new_cap;
    }

    vector->elements[vector->size] = token;
    vector->size++;

    return true;
}

void Wave_TokenString_EscOrExprVector_delete(struct Wave_TokenFstr_EscOrExprVector *const vector) {
    free(vector->elements);
    vector->elements = NULL;
    vector->size = 0;
    vector->capacity = 0;
}
