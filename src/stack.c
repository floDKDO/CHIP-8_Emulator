#include <stdio.h>
#include <string.h>

#include "stack.h"

struct stack create_stack(void)
{
    struct stack s;
    memset(s.addresses, 0, MAX_STACK_SIZE * sizeof(int16_t));
    s.actual_top = 0;
    return s;
}

struct stack push_stack(struct stack s, int16_t address)
{
    if(s.actual_top < MAX_STACK_SIZE)
    {
        s.addresses[s.actual_top] = address;
        s.actual_top += 1;
    }
    else fprintf(stderr, "The stack is full!\n");

    return s;
}

struct stack pop_stack(struct stack s)
{
    if(s.actual_top > 0)
    {
        s.addresses[s.actual_top] = 0;
        s.actual_top -= 1;
    }
    else fprintf(stderr, "The stack is empty!\n");

    return s;
}

int16_t get_top_stack(struct stack s)
{
    return s.addresses[s.actual_top];
}

void print_stack(struct stack s)
{
    for(int i = s.actual_top - 1; i >= 0; --i)
    {
        printf("Position: %d, value: %d\n", i, s.addresses[i]);
    }
}
