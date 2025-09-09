#ifndef __CPU__
#define __CPU__

#include "memory.h"


struct cpu
{
    unsigned char program_counter;
    unsigned char index_register;
    unsigned char v_registers[16];
};

void read_instructions(struct memory m);


#endif // __CPU__
