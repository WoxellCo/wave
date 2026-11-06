#include <wave/token.h>
#include <stdlib.h>

struct Wave_TokenVector Wave_TokenVector_new(struct Wave_Context *const context, const size_t capacity) {
    struct Wave_TokenVector vector = (struct Wave_TokenVector){
        .tokens = NULL,
        .size = 0,
        .capacity = 0,
    };

    if (capacity == 0)
        return vector;

    vector.tokens = (struct Wave_Token *)w_malloc(context->allocator.alloc, context->allocator.userdata, capacity * sizeof(struct Wave_Token));
    
    if (vector.tokens != NULL)
        vector.capacity = capacity;

    return vector;
}

bool Wave_TokenVector_push(struct Wave_Context *const context, struct Wave_TokenVector *const vector, const struct Wave_Token token) {
    if (vector->size >= vector->capacity) {
        const size_t new_cap = vector->capacity * 2;
        struct Wave_Token *const reallocated = (struct Wave_Token *)w_realloc(context->allocator.alloc, context->allocator.userdata, vector->tokens, vector->capacity * sizeof(struct Wave_Token), new_cap * sizeof(struct Wave_Token));
        if (reallocated == NULL)
            return false;
        vector->tokens = reallocated;
        vector->capacity = new_cap;
    }

    vector->tokens[vector->size] = token;
    vector->size++;

    return true;
}

void Wave_TokenVector_delete(struct Wave_Context *const context, struct Wave_TokenVector *const vector) {
    if (vector == NULL)
        return;

    for (size_t i = 0; i < vector->size; i++) {
        Wave_Token_delete(context, &vector->tokens[i]);
    }

    w_free(context->allocator.alloc, context->allocator.userdata, vector->tokens, vector->size * sizeof(struct Wave_Token));
    vector->tokens = NULL;
    vector->size = 0;
    vector->capacity = 0;
}

bool Wave_Token_do_counters_match(const struct Wave_TokenCounters *const counters) {
    return
        counters->lparen == counters->rparen &&
        counters->lbrace == counters->rbrace &&
        counters->lbrack == counters->rbrack;
}

// mk: frees the memory of any token with heap allocation, any token can be passed
void Wave_Token_delete(struct Wave_Context *const context, struct Wave_Token *const token) {
    if (token == NULL)
        return;

    if (token->type == Wave_TokenType_LIT_STR) {
        Wave_String_delete(context, &token->data.str);
    }
}

const char *Wave_Token_keyword_to_string(const enum Wave_TokenType token_type) {
    switch (token_type) {
        case Wave_TokenType_LET     :   return "let"    ;
        case Wave_TokenType_CONST   :   return "const"  ;
        case Wave_TokenType_FN      :   return "fn"     ;
        case Wave_TokenType_TYPEOF  :   return "typeof" ;
        case Wave_TokenType_MATCH   :   return "match"  ;
        default                     :   return ""       ;
    }
}

const char *Wave_Token_to_string(const enum Wave_TokenType token_type) {
    switch (token_type) {
        case Wave_TokenType_LET         :   return "let"    ;
        case Wave_TokenType_CONST       :   return "const"  ;
        case Wave_TokenType_FN          :   return "fn"     ;
        case Wave_TokenType_TYPEOF      :   return "typeof" ;
        case Wave_TokenType_MATCH       :   return "match"  ;
        case Wave_TokenType_COLON       :   return ":"      ;
        case Wave_TokenType_SEMICOLON   :   return ";"      ;
        case Wave_TokenType_ASS         :   return "="      ;
        case Wave_TokenType_EQ          :   return "=="     ;
        case Wave_TokenType_SEQ         :   return "==="    ;
        case Wave_TokenType_AWA         :   return ">~<"    ;
        default                         :   return ""       ;
    }
}
