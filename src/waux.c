#include <waux.h>
#include <stdlib.h>

void *w_alloc(void *userdata, void *ptr, size_t old_size, size_t new_size) {
    (void)userdata;
    (void)old_size;
    if (ptr == NULL) {
        if (new_size > 0)
            return malloc(new_size);
    } if (new_size == 0) {
        if (ptr != NULL)
            free(ptr);
    } else {
        return realloc(ptr, new_size);
    }
    return NULL;
}