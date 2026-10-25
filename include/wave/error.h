#ifndef WAVE_ERROR_H
#define WAVE_ERROR_H

#include <wave/def.h>

struct Wave_String Wave_Error_compilation_error_to_string(
    const struct Wave_CharLocation *const coords,
    const char *const source,
    const struct Wave_CompilationError *const err
);

#endif