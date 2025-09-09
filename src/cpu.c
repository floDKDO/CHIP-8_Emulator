#include "cpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

void read_instructions(struct memory m)
{
    m.program_counter = &(m.memory_array[START_ADDRESS]);

    while(m.program_counter != &(m.memory_array[RAM_SIZE]))
    {
        uint16_t instruction = (*(m.program_counter) << 8) | (*(m.program_counter + 1)); //Big endian
        printf("%04x ", instruction);
        switch(instruction)
        {
            case 0x00E0:
                printf("(Clear screen)\n");
                break;

            default:
                printf("(Unknown for now)\n");
                break;
        }
        m.program_counter += 2;
    }
}
