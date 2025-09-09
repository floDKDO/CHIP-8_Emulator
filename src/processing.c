#include "processing.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>


struct processing create_memory_and_cpu(void)
{
    struct processing p;
    memset(p.memory.memory_array, 0, RAM_SIZE * sizeof(char));
    p.cpu.program_counter = &(p.memory.memory_array[START_ADDRESS]);
    p.cpu.index_register = 0;
    memset(p.cpu.v_registers, 0, 16 * sizeof(unsigned char));
    return p;
}

struct processing load_program_in_memory(struct processing p, char* pathname)
{
    FILE* rom;
    if((rom = fopen(pathname, "r")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fread(p.memory.memory_array + START_ADDRESS, sizeof(char), RAM_SIZE, rom);

    return p;
}

void dump_memory(struct processing p)
{
    for(int i = 0; i < RAM_SIZE; ++i)
    {
        if(i == START_ADDRESS)
        {
            printf("###START OF PROGRAM###\n");
        }
        printf("%d: %02x\n", i, p.memory.memory_array[i]);
    }
}

//m.program_counter = &(m.memory_array[START_ADDRESS]);

void read_instructions(struct processing p)
{
    p.cpu.program_counter = &(p.memory.memory_array[START_ADDRESS]);

    while(p.cpu.program_counter != &(p.memory.memory_array[RAM_SIZE]))
    {
        uint16_t instruction = (*(p.cpu.program_counter) << 8) | (*(p.cpu.program_counter + 1)); //Big endian
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
        p.cpu.program_counter += 2;
    }
}
