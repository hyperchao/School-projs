#ifndef STACK_H__
#define STACK_H__

#define STACK_INIT_SIZE 256

typedef struct {
    void *a;
    size_t size, top;
} stack_t[1];

#define stack_push_v(stack, value, type)  *(type*)stack_push_raw(stack, sizeof(type)) = (value)

#define stack_pop_v(stack, type) (*(type*)stack_pop_raw(stack, sizeof(type)))

#define stack_top_v(stack, type) (*(type*)stack_top_raw(stack, sizeof(type)))

#define stack_isempty(stack) ((stack)->top == 0)

#define stack_isfull(stack, capacity) ((stack)->top >= (capacity))

#define stack_size(stack) ((stack)->top)

#define stack_clear(stack) (stack)->top = 0

void stack_init(stack_t s, size_t size);

void *stack_push_raw(stack_t s, size_t size);

void *stack_pop_raw(stack_t s, size_t size);

void *stack_top_raw(stack_t s, size_t size);

void stack_free(stack_t s);

#endif /* STACK_H__ */