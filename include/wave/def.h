#ifndef WAVE_DEFS_H
#define WAVE_DEFS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <wave/string.h>
#include <wave/log.h>

#define w_malloc(f, ud, size) ((f)((ud), NULL, 0, (size)))
#define w_realloc(f, ud, ptr, osz, size) ((f)((ud), (ptr), (osz), (size)))
#define w_free(f, ud, ptr, osz) ((f)((ud), (ptr), (osz), 0))

/*union Wave_TokenData {
    
};*/


struct Wave_Identifier {
    struct Wave_String id;
};



// mk: temporary defs for debug
enum Wave_LogSignalType Wave_debugger_log(const char *const file, const size_t line);

//#define Wave_CompilationErrorType_UnexpectedCharacter Wave_debugger_log(__FILE__, __LINE__)

#endif
