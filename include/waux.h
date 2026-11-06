#ifndef WAUX_H
#define WAUX_H

#include <wave/context.h>

// mk: at least for now, the wave auxiliary library maps the allocator directly to the classic libc allocators

void *w_alloc(void *userdata, void *ptr, size_t old_size, size_t new_size);

#endif