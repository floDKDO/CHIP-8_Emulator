#ifndef __STACK__
#define __STACK__

#define MAX_STACK_SIZE 1024

#include <stdint.h>

struct stack
{
    uint16_t addresses[MAX_STACK_SIZE];
    unsigned int actual_top;
};

struct stack create_stack(void);
struct stack push_stack(struct stack s, uint16_t address);
struct stack pop_stack(struct stack s, uint16_t* program_counter);
uint16_t get_top_stack(struct stack s);
void print_stack(struct stack s);

#endif // __STACK__
