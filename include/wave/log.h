#ifndef WAVE_LOG_H
#define WAVE_LOG_H

#include <wave/context.h>
#include <wave/string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// mk: type of errors during compilation
enum Wave_LogSignalType {
    Wave_LogSignalType_None = 0,
    Wave_LogSignalType_InvalidAllocation,
    Wave_LogSignalType_UnexpectedNullPointer,
    Wave_LogSignalType_UnexpectedEndOfFile,
    Wave_LogSignalType_UnexpectedBoundary,
    Wave_LogSignalType_InvalidStringBoundary,
    Wave_LogSignalType_InvalidSymbol,
    Wave_LogSignalType_UnexpectedCharacter,
    Wave_LogSignalType_NumberOutOfRange,
    Wave_LogSignalType_InvalidFloatToIntLiteral,
    Wave_LogSignalType_InvalidUtf8Character,
    Wave_LogSignalType_InvalidValue,
    Wave_LogSignalType_InvalidSequence,
    Wave_LogSignalType_UnknownError,
    Wave_LogSignalType_UnexpectedToken,
};

enum Wave_LogSignalInfoType {
    Wave_LogSignalInfoType_None,
    Wave_LogSignalInfoType_ExpectedSequence
};

// mk: errors such as allocation error etc, that occur during compilation
struct Wave_LogSignal {
    enum Wave_LogSignalType type;
    // mk: i put in a struct because there might be additional useful diagnostics info in the future
    enum Wave_LogSignalInfoType info_type;
    union {
        uint8_t none;
        char expected_sequence[16];
    } info;
};

#define Wave_LogSignal_none {.type = Wave_LogSignalType_None, .info = {0}, .info_type = Wave_LogSignalInfoType_None}
#define Wave_LogSignal_is_error(err) ((err).type != Wave_LogSignalType_None)

// mk: wave lexers can only emit only one error at a time, everything below here is for anything that comes after the lexing stage

enum Wave_LogType {
    Wave_LogType_Error,
    Wave_LogType_Warning,
    Wave_LogType_InternalError, // mk: type of errors that are not caused by the user, but by the system itself
};

// mk: please not another vector bruh.
struct Wave_LogVector {
    struct Wave_Log *logs;
    size_t size;
    size_t capacity;
};
// mk: i wish c99 had generics, using macros feels incredibly hacky and unstable, so i'm gonna stick with manual implementation

struct Wave_Log {
    enum Wave_LogType type;
    struct Wave_LogSignal signal;
    struct Wave_String message;
};

void Wave_Log_delete(struct Wave_Context *const context, struct Wave_Log *const log);

struct Wave_LogVector Wave_LogVector_new(struct Wave_Context *const context, const size_t capacity);
// mk: passed as pointer because `Wave_Log` is 64 bytes, makes a shallow copy in the vector and it's moved though
bool Wave_LogVector_reserve(struct Wave_Context *const context, struct Wave_LogVector *const vector, const size_t capacity);
bool Wave_LogVector_push(struct Wave_Context *const context, struct Wave_LogVector *const vector, const struct Wave_Log *const log);
void Wave_LogVector_delete(struct Wave_Context *const context, struct Wave_LogVector *const vector);

#endif
