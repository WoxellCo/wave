#ifndef WAUX_H
#define WAUX_H

#include <wave/context.h>

// mk: at least for now, the wave auxiliary library maps the allocator directly to the classic libc allocators
#define w_malloc(size) w_alloc(NULL, NULL, 0, (size))
#define w_realloc(ptr, size) w_alloc(NULL, (ptr), 0, (size))
#define w_free(ptr) w_alloc(NULL, (ptr), 0, 0)

void *w_alloc(void *userdata, void *ptr, size_t old_size, size_t new_size);

#endif