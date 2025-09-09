#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct memory create_memory(void)
{
    struct memory m;
    memset(m.memory_array, 0, RAM_SIZE * sizeof(char));
    m.program_counter = &(m.memory_array[START_ADDRESS]);
    return m;
}

struct memory load_program_in_memory(struct memory m, char* pathname)
{
    FILE* rom;
    if((rom = fopen(pathname, "r")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fread(m.memory_array + START_ADDRESS, sizeof(char), RAM_SIZE, rom);

    return m;
}

void dump_memory(struct memory m)
{
    for(int i = 0; i < RAM_SIZE; ++i)
    {
        if(i == START_ADDRESS)
        {
            printf("###START OF PROGRAM###\n");
        }
        printf("%d: %02x\n", i, m.memory_array[i]);
    }
}
