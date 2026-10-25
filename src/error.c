#include <wave/def.h>
#include <stdio.h>
#include <stdlib.h>
#include <wave/error.h>

struct Wave_String Wave_Error_compilation_error_to_string(
    const struct Wave_CharLocation *const coords,
    const char *const source,
    const struct Wave_CompilationError *const err
) {
    char buffer[128];
    struct Wave_String message = Wave_String_null;//Wave_String_new(64);

    switch (err->type) {
        case Wave_CompilationErrorType_InvalidAllocation:
            // mk: temporary `sprintf`
            snprintf(buffer, 128, "wave system error: invalid allocation");
            break;
        case Wave_CompilationErrorType_UnexpectedNullPointer:
            // mk: temporary `sprintf`
            snprintf(buffer, 128, "internal wave compiler error: unexpected null pointer");
            break;
        case Wave_CompilationErrorType_UnexpectedEndOfFile:
            snprintf(buffer, 128, "%s:%zu:%zu: error: unexpected EOF, `%s` expected",
                coords->path,
                coords->row + 1,
                coords->column + 1,
                err->info.expected_sequence
            );
            break;
        case Wave_CompilationErrorType_UnexpectedBoundary:
            snprintf(buffer, 128, "%s:%zu:%zu: error: unexpected boundary `%.*s`, `%s` expected",
                coords->path,
                coords->row + 1,
                coords->column + 1,
                10,
                source + coords->index,
                err->info.expected_sequence
            );
            break;
        case Wave_CompilationErrorType_None:
            return Wave_String_null;
        default:
            break;
    }

    message = Wave_String_from(buffer);

    if (Wave_String_is_null(message)) {
        exit(1);
    }

    return message;
}