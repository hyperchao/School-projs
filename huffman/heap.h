#ifndef HEAP_H__
#define HEAP_H__

#include "stack.h" /* stack_t and related functions */

typedef int (*heap_cmp_func)(const void *a, const void *b);

typedef struct {
    stack_t s;
    void *cache;
    heap_cmp_func cmp;
    size_t len, size;
} heap_t[1];

#define heap_push_v(heap, value, type)  do { *(type*)(heap)->cache = (value); heap_push_raw(heap); } while(0)

#define heap_pop_v(heap, type) (*(type*)heap_pop_raw(heap))

#define heap_top_v(heap, type) (*(type*)(heap)->s->a)

#define heap_array(heap) ((heap)->s->a)

#define heap_isempty(heap) ((heap)->len == 0)

#define heap_isfull(heap, capacity) ((heap)->len >= (capacity))

#define heap_len(heap) ((heap)->len)

#define heap_clear(heap) do { (heap)->len = 0; (heap)->s->top = 0; } while(0)

void heap_init(heap_t h, size_t len, size_t size, heap_cmp_func cmp);

void *heap_pop_raw(heap_t h);
                           
void heap_push_raw(heap_t h);

void heap_free(heap_t h);

#endif /* HEAP_H__ */