#include <wave/context.h>
#include <wave/string.h>
#include <stdbool.h>
#include <stddef.h>
#include <wave/log.h>
#include <wave/def.h>

struct Wave_LogVector Wave_LogVector_new(struct Wave_Context *const context, const size_t capacity) {
    struct Wave_LogVector vector = {
        .logs = NULL,
        .size = 0,
        .capacity = 0
    };

    if (capacity > 0) {
        vector.logs = w_malloc(
            context->allocator.alloc,
            context->allocator.userdata,
            capacity * sizeof(struct Wave_Log)
        );
        if (vector.logs != NULL)
            vector.capacity = capacity;
    }

    return vector;
}

bool Wave_LogVector_reserve(struct Wave_Context *const context, struct Wave_LogVector *const vector, const size_t capacity) {
    if (vector == NULL)
        return false;

    if (vector->logs == NULL) {
        *vector = Wave_LogVector_new(context, capacity);
        return vector->logs != NULL;
    }

    if (capacity > vector->capacity) {
        struct Wave_Log *const reallocated = w_realloc(
            context->allocator.alloc,
            context->allocator.userdata,
            vector->logs,
            vector->capacity * sizeof(struct Wave_Log),
            capacity * sizeof(struct Wave_Log)
        );

        if (reallocated == NULL)
            return false;

        vector->logs = reallocated;
        vector->capacity = capacity;
    }

    return true;
}

bool Wave_LogVector_push(struct Wave_Context *const context, struct Wave_LogVector *const vector, const struct Wave_Log *const log) {
    if (vector == NULL)
        return false;
    if (log == NULL)
        return false;

    if (vector->capacity == 0) {
        *vector = Wave_LogVector_new(context, 4);
        if (vector->logs == NULL)
            return false;
    } else if (vector->size >= vector->capacity) {
        if (!Wave_LogVector_reserve(context, vector, vector->capacity * 2))
            return false;
    }

    vector->logs[vector->size] = *log;
    vector->size++;
    return true;
}

void Wave_LogVector_delete(struct Wave_Context *const context, struct Wave_LogVector *const vector) {
    if (vector == NULL)
        return;

    for (size_t i = 0; i < vector->size; i++)
        Wave_Log_delete(context, &vector->logs[i]);

    w_free(
        context->allocator.alloc,
        context->allocator.userdata,
        vector->logs,
        vector->capacity * sizeof(struct Wave_Log)
    );
}

void Wave_Log_delete(struct Wave_Context *const context, struct Wave_Log *const log) {
    if (log == NULL)
        return;
    Wave_String_delete(context, &log->message);
}
