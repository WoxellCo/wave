#ifndef WAVE_INTERNAL_CONTEXT_H
#define WAVE_INTERNAL_CONTEXT_H

#include <stddef.h>

typedef void *(*Wave_Alloc)(void *userdata, void *ptr, size_t old_size, size_t new_size);

struct Wave_Allocator {
    Wave_Alloc alloc;
    void *userdata;
};

struct Wave_Context {
    struct Wave_Allocator *allocator
};

#endif