#include <stdlib.h>
#include <assert.h>
#include "stack.h"

void stack_init(stack_t s, size_t size) {
    s->size = size?size:STACK_INIT_SIZE;
    s->a = malloc(s->size);
    s->top = 0;
}

void *stack_push_raw(stack_t s, size_t size) {
    assert(s->a && size > 0);
    void *ret;
    if (s->top + size > s->size) {
        while (s->top + size > s->size) 
            s->size += s->size >> 1;
        s->a = realloc(s->a, s->size);
        assert(s->a && "realloc failure");
    }
    ret = (char*)s->a + s->top;
    s->top += size;
    return ret;
}

void *stack_top_raw(stack_t s, size_t size) {
    assert(s->top >= size);
    return (char*)s->a + s->top - size;
}

void *stack_pop_raw(stack_t s, size_t size) {
    assert(s->top >= size);
    return (char*)s->a + (s->top -= size);
}

void stack_free(stack_t s) {
    free(s->a);
}