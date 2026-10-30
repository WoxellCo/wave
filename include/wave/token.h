#ifndef WAVE_TOKEN_H
#define WAVE_TOKEN_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <wave/def.h>
#include <wave/numeric.h>
#include <wave/char-location.h>

enum Wave_TokenType {
    Wave_TokenType_ID,          // identifier
    Wave_TokenType_LET,         // `let`
    Wave_TokenType_CONST,       // `const`
    Wave_TokenType_COLON,       // `:`
    Wave_TokenType_SEMICOLON,   // `;`
    Wave_TokenType_COMMA,       // `,`
    Wave_TokenType_DOT,         // `.`
    Wave_TokenType_ASS,         // `=`
    Wave_TokenType_EQ,          // `==`
    Wave_TokenType_SEQ,         // `===` (strict equal)
    Wave_TokenType_NEQ,         // `!=`
    Wave_TokenType_SNEQ,        // `!==`
    Wave_TokenType_GT,          // `>`
    Wave_TokenType_GTE,         // `>=`
    Wave_TokenType_LT,          // `<`
    Wave_TokenType_LTE,         // `<=`
    Wave_TokenType_ARROW,       // `->`
    Wave_TokenType_FARROW,      // `=>`
    Wave_TokenType_LIT_INT,     // int literal
    Wave_TokenType_LIT_FLOAT,   // float literal
    Wave_TokenType_LIT_STR,     // string literal
    Wave_TokenType_BOOL_TRUE,   // bool literal `true`
    Wave_TokenType_BOOL_FALSE,  // bool literal `false`
    Wave_TokenType_TY_BOOL,     // `bool`
    Wave_TokenType_TY_I8,       // `i8`
    Wave_TokenType_TY_I16,      // `i16`
    Wave_TokenType_TY_I32,      // `i32`
    Wave_TokenType_TY_I64,      // `i64`
    Wave_TokenType_TY_I128,     // `i128`
    Wave_TokenType_TY_U8,       // `u8`
    Wave_TokenType_TY_U16,      // `u16`
    Wave_TokenType_TY_U32,      // `u32`
    Wave_TokenType_TY_U64,      // `u64`
    Wave_TokenType_TY_U128,     // `u128`
    Wave_TokenType_TY_F32,      // `f32`
    Wave_TokenType_TY_F64,      // `f64`
    Wave_TokenType_TY_V0,       // `v0`
    Wave_TokenType_TY_VOID,     // `void`
    Wave_TokenType_TY_STRING,   // `string`
    Wave_TokenType_AUTO,        // `auto`
    Wave_TokenType_TYPENAME,    // `typename`
    Wave_TokenType_TYPEOF,      // `typeof`
    Wave_TokenType_INSTANCEOF,  // `instanceof`
    Wave_TokenType_SIZEOF,      // `sizeof`
    Wave_TokenType_TEMPLATE,    // `template`
    Wave_TokenType_MUT,         // `mut`
    Wave_TokenType_FN,          // `fn`
    Wave_TokenType_USE,         // `use`
    Wave_TokenType_IMPORT,      // `import`
    Wave_TokenType_TYPE,        // `type`
    Wave_TokenType_CLASS,       // `class`
    Wave_TokenType_STRUCT,      // `struct`
    Wave_TokenType_ENUM,        // `enum`
    Wave_TokenType_PUBLIC,      // `public` (reserved)
    Wave_TokenType_PUB,         // `pub` (reserved)
    Wave_TokenType_PRIVATE,     // `private` (reserved)
    Wave_TokenType_PRIV,        // `priv` (reserved)
    Wave_TokenType_PROTECTED,   // `protected` (reserved)
    Wave_TokenType_INTERFACE,   // `interface` (reserved)
    Wave_TokenType_IF,          // `if`
    Wave_TokenType_ELSE,        // `else`
    Wave_TokenType_ELIF,        // `elif`
    Wave_TokenType_SWITCH,      // `switch`
    Wave_TokenType_CASE,        // `case`
    Wave_TokenType_DEFAULT,     // `default`
    Wave_TokenType_LOOP,        // `loop`
    Wave_TokenType_FOR,         // `for`
    Wave_TokenType_WHILE,       // `while`
    Wave_TokenType_DO,          // `do`
    Wave_TokenType_MATCH,       // `match`
    Wave_TokenType_BREAK,       // `break`
    Wave_TokenType_CONTINUE,    // `continue`
    Wave_TokenType_RETURN,      // `return`
    Wave_TokenType_GOTO,        // `goto`
    Wave_TokenType_MODULE,      // `module`
    Wave_TokenType_MOD,         // `mod`
    Wave_TokenType_IMPL,        // `impl`
    Wave_TokenType_TRAIT,       // `trait`
    Wave_TokenType_MOVE,        // `move`
    Wave_TokenType_EXPORT,      // `export`
    Wave_TokenType_NAMESPACE,   // `namespace`
    Wave_TokenType_GLOBAL,      // `global`
    Wave_TokenType_LOCAL,       // `local`
    Wave_TokenType_NULL,        // `null`
    Wave_TokenType_UNDEFINED,   // `undefined`
    Wave_TokenType_ASYNC,       // `async`
    Wave_TokenType_AWAIT,       // `await`
    Wave_TokenType_DYN,         // `dyn`
    Wave_TokenType_EXTERN,      // `extern`
    Wave_TokenType_IN,          // `in`
    Wave_TokenType_REF,         // `ref`
    Wave_TokenType_SELF,        // `self`
    Wave_TokenType_THIS,        // `this`
    Wave_TokenType_STATIC,      // `static`
    Wave_TokenType_UNSAFE,      // `unsafe`
    Wave_TokenType_ABSTRACT,    // `abstract`
    Wave_TokenType_BECOME,      // `become`
    Wave_TokenType_BOX,         // `box`
    Wave_TokenType_FINAL,       // `final`
    Wave_TokenType_GEN,         // `gen`
    Wave_TokenType_MACRO,       // `macro`
    Wave_TokenType_OVERRIDE,    // `override`
    Wave_TokenType_TRY,         // `try`
    Wave_TokenType_CATCH,       // `catch`
    Wave_TokenType_FINALLY,     // `finally`
    Wave_TokenType_VIRTUAL,     // `virtual`
    Wave_TokenType_YIELD,       // `yield`
    Wave_TokenType_QMARK,       // `?`
    Wave_TokenType_EMARK,       // `!`
    Wave_TokenType_DOUBLE_OR,   // `||`
    Wave_TokenType_DOUBLE_AND,  // `&&`
    Wave_TokenType_OR,          // `|`
    Wave_TokenType_AND,         // `&`
    Wave_TokenType_XOR,         // `^`
    Wave_TokenType_NOT,         // `~`
    Wave_TokenType_MUL,         // `*`
    Wave_TokenType_DIV,         // `/`
    Wave_TokenType_ADD,         // `+`
    Wave_TokenType_MIN,         // `-`
    Wave_TokenType_LSHIFT,      // `<<`
    Wave_TokenType_RSHIFT,      // `>>`
    Wave_TokenType_ASS_ADD,     // `+=`
    Wave_TokenType_ASS_MIN,     // `-=`
    Wave_TokenType_ASS_MUL,     // `*=`
    Wave_TokenType_ASS_DIV,     // `/=`
    Wave_TokenType_ASS_LSHIFT,  // `<<=`
    Wave_TokenType_ASS_RSHIFT,  // `>>=`
    Wave_TokenType_ASS_OR,      // `|=`
    Wave_TokenType_ASS_AND,     // `&=`
    Wave_TokenType_ASS_XOR,     // `^=`
    Wave_TokenType_ASS_NOT,     // `~=`
    Wave_TokenType_ASS_DOUBLE_AND,// `&&=`
    Wave_TokenType_ASS_DOUBLE_OR,// `||=`
    Wave_TokenType_LPAREN,      // `(`
    Wave_TokenType_RPAREN,      // `)`
    Wave_TokenType_LBRACK,      // `[`
    Wave_TokenType_RBRACK,      // `]`
    Wave_TokenType_LBRACE,      // `{`
    Wave_TokenType_RBRACE,      // `}`
    Wave_TokenType_RANGE,       // `..` or `..=` <- just this | complete exmples: `0..9` or `0..=9` or `..=9` or `x..y` or `..` ...
    Wave_TokenType_FSTR,        // `$"hello {world}!"`
    Wave_TokenType_NEW,         // `new`
    Wave_TokenType_DELETE,      // `delete`
    Wave_TokenType_AS,          // `as`
};

struct Wave_TokenCounters {
    size_t lparen;
    size_t rparen;
    size_t lbrack;
    size_t rbrack;
    size_t lbrace;
    size_t rbrace;
};

struct Wave_TokenVector {
    struct Wave_Token *tokens;
    size_t size;
    size_t capacity;
};

struct Wave_TokenVector Wave_TokenVector_new(const size_t capacity);
bool Wave_TokenVector_push(struct Wave_TokenVector *const vector, const struct Wave_Token token);
void Wave_TokenVector_delete(struct Wave_TokenVector *const vector);

struct Wave_Token {
    enum Wave_TokenType type;
    struct Wave_CharLocation location;
    size_t length;
    union {
        uint8_t none;
        struct Wave_String str;
        struct Wave_Numeric num;
    } data;
};

bool Wave_Token_do_counters_match(const struct Wave_TokenCounters *const counters);

#endif
