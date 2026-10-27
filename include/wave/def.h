#ifndef WAVE_DEFS_H
#define WAVE_DEFS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <wave/string.h>



/*union Wave_TokenData {
    
};*/


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
    Wave_CompilationErrorType_InvalidUtf8Character,
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

struct Wave_Identifier {
    struct Wave_String id;
};

#endif