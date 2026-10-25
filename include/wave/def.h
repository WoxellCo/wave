#ifndef WAVE_DEFS_H
#define WAVE_DEFS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>



struct Wave_String {
    char *str;
    size_t length;
    size_t capacity;
};

/*union Wave_TokenData {
    
};*/

// mk: determines which row and column is a specific character, useful for error handling
struct Wave_CharLocation {
    char *path;
    size_t row, column;
    size_t index;
};

// mk: type of errors during compilation
enum Wave_CompilationErrorType {
    Wave_CompilationErrorType_None = 0,
    Wave_CompilationErrorType_InvalidAllocation,
    Wave_CompilationErrorType_UnexpectedNullPointer,
    Wave_CompilationErrorType_UnexpectedEndOfFile,
    Wave_CompilationErrorType_UnexpectedBoundary,
    Wave_CompilationErrorType_InvalidStringBoundary,
    Wave_CompilationErrorType_InvalidSymbol,
    Wave_CompilationErrorType_UnexpectedCharacter,
    Wave_CompilationErrorType_NumberOutOfRange,
    Wave_CompilationErrorType_InvalidFloatToIntLiteral,
};

enum Wave_CompilationErrorInfoType {
    Wave_CompilationErrorInfoType_None,
    Wave_CompilationErrorInfoType_ExpectedSequence
};

// mk: errors such as allocation error etc, that occur during compilation
struct Wave_CompilationError {
    enum Wave_CompilationErrorType type;
    // mk: i put in a struct because there might be additional useful diagnostics info in the future
    enum Wave_CompilationErrorInfoType info_type;
    union {
        uint8_t none;
        char expected_sequence[16];
    } info;
};

#define Wave_CompilationError_none {.type = Wave_CompilationErrorType_None, .info = {0}, .info_type = Wave_CompilationErrorInfoType_None}

#define Wave_String_null ((struct Wave_String){NULL, 0, 0})
#define Wave_String_is_null(s) ((s).str == NULL && (s).capacity == 0)

struct Wave_String Wave_String_new(const size_t capacity);
struct Wave_String Wave_String_from(const char *const str);
struct Wave_String Wave_String_from_and_max_len(const char *const str, const size_t max_len);
void Wave_String_delete(struct Wave_String *const s);
bool Wave_String_push(struct Wave_String *const s, const char ch);
bool Wave_String_append(struct Wave_String *const s, const char *const str);

struct Wave_Identifier {
    struct Wave_String id;
};

#endif