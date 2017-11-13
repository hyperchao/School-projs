#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "heap.h"

void heap_init(heap_t h, size_t len, size_t size, heap_cmp_func cmp) {
    h->len = 0;
    h->size = size;
    h->cmp = cmp;
    h->cache = (void*)malloc(size);
    stack_init(h->s, len*size);
}

#define AT(n) (a + (n)*size)
void *heap_pop_raw(heap_t h) {
    assert(h->len > 0);
    size_t n = h->len, size = h->size;
    h->len--;
    if (n == 1)
        return stack_pop_raw(h->s, size);
    
    size_t i = 0, child;
    char *a = (char*)h->s->a;
    heap_cmp_func cmp = h->cmp;
    memcpy(h->cache, a, size);
    while ((child = 2*i + 1) < n) {
        if (child + 1 < n && cmp(AT(child + 1), AT(child)) < 0)
            child++;
        if (cmp(AT(child), AT(n - 1)) >= 0)
            break;
        memcpy(AT(i), AT(child), size);
        i = child;
    }
    memcpy(AT(i), AT(n - 1), size);
    stack_pop_raw(h->s, size);
    return h->cache;
}

void heap_push_raw(heap_t h) {
    assert(h->s->a && h->s->size);
    size_t i = h->len++, parent, size = h->size;
    heap_cmp_func cmp = h->cmp;
    stack_push_raw(h->s, h->size);
    char *a = (char*)h->s->a;
    while (i > 0) {
        parent = (i - 1)/2;
        if (cmp(AT(parent), h->cache) <= 0)
            break;
        memcpy(AT(i), AT(parent), size);
        i = parent;
    }
    memcpy(AT(i), h->cache, size);
}

void heap_free(heap_t h) {
    stack_free(h->s);
    free(h->cache);
}
