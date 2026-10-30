#ifndef WAVE_FSTR_H
#define WAVE_FSTR_H

#include <stddef.h>
#include <wave/def.h>
#include <wave/token.h>
#include <wave/lexer.h>
#include <wave/token-string.h> // mk: `string` and `fstr` have a lot of properties in common, so a few things are borrowed from there

// mk: the {...} inside $"...", braces `{}` are included in `start_index` and `length`, but in the elements index the braces are *excluded*!
struct Wave_TokenFstr_Expr {
    size_t start_index;                                     // mk: very first character index, like the index of `{` opening the expr
    size_t length;                                          // mk: *total* length (braces included)
    uint8_t bonudary_left_length, bonudary_right_length;    // mk: most cases would be one, because both `{` and `}` occupy one char
    struct Wave_TokenVector tokens;                         // mk: literally wave lives here, inside the expr
};

union Wave_TokenFstr_EscOrExpr {
    struct Wave_TokenString_Esc esc;
    struct Wave_TokenFstr_Expr expr;
};

enum Wave_TokenFstr_ElementType {
    Wave_TokenFstr_ElementType_Esc,
    Wave_TokenFstr_ElementType_Expr,
};

struct Wave_TokenFstr_Element {
    union Wave_TokenFstr_EscOrExpr data;
    enum Wave_TokenFstr_ElementType type;
};

struct Wave_TokenFstr_EscOrExprVector {
    struct Wave_TokenFstr_Element *elements;
    size_t size;
    size_t capacity;
};

// mk: token for the fstr literal
struct Wave_TokenFstr {
    struct Wave_String string_data;
    struct Wave_TokenString_EscVector sequence_vector;
    struct Wave_TokenFstr_Expr tokens;
};
/* mk: example: if you have $"hello \n {world}!", `string_data` stores "hello⎵
⎵!", sequence vector has an element at index `6` of the string, and an expr at index `8`
*/


// mk: the following function runs when 
// mk: this function should trigger whenever it hits a `}` or even `:` (used for formatting)
//bool Wave_Fstr_expr_delimeter_predicate(const Wave_Lexer_DelimiterPredicate user_delimiter_predicate);
bool Wave_Fstr_expr_delimeter_predicate(
    const struct Wave_CharLocation *const coords,
    const struct Wave_TokenCounters *const token_counters,
    const char *const source,
    const size_t n,
    const struct Wave_Lexer_EncapsulatedDelimiterPredicate *const chain
);

struct Wave_TokenFstr_EscOrExprVector Wave_TokenString_EscOrExprVector_new(const size_t capacity);
bool Wave_TokenFstr_EscOrExprVector_push(struct Wave_TokenFstr_EscOrExprVector *const vector, const struct Wave_TokenFstr_Element token);
void Wave_TokenFstr_EscOrExprVector_delete(struct Wave_TokenFstr_EscOrExprVector *const vector);

#endif
