#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "defs.h"
#include "chip_8.h"


struct chip_8 create_chip_8(void)
{
    struct chip_8 c;

    NCHK(c.display.window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN));
    NCHK(c.display.renderer = SDL_CreateRenderer(c.display.window, -1, SDL_RENDERER_PRESENTVSYNC));
    CHK(SDL_RenderSetLogicalSize(c.display.renderer, DISPLAY_WIDTH, DISPLAY_HEIGHT));
    c.display.rect.x = 0;
    c.display.rect.y = 0;
    c.display.rect.w = DISPLAY_WIDTH;
    c.display.rect.h = DISPLAY_HEIGHT;

    memset(c.memory.memory_array, 0, RAM_SIZE * sizeof(char));

    c.cpu.program_counter = &(c.memory.memory_array[START_ADDRESS]);
    c.cpu.index_register = 0;
    memset(c.cpu.v_registers, 0, 16 * sizeof(unsigned char));

    c.stack = create_stack();

    return c;
}

struct chip_8 load_program_in_memory(struct chip_8 c, char* pathname)
{
    FILE* rom;
    if((rom = fopen(pathname, "r")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fread(c.memory.memory_array + START_ADDRESS, sizeof(char), RAM_SIZE, rom);

    return c;
}

void dump_memory(struct chip_8 c)
{
    for(int i = 0; i < RAM_SIZE; ++i)
    {
        if(i == START_ADDRESS)
        {
            printf("###START OF PROGRAM###\n");
        }
        printf("%d: %02x\n", i, c.memory.memory_array[i]);
    }
}

void handle_instructions(struct chip_8 c)
{
    c.cpu.program_counter = &(c.memory.memory_array[START_ADDRESS]);

    while(c.cpu.program_counter != &(c.memory.memory_array[RAM_SIZE]))
    {
        uint16_t instruction = (*(c.cpu.program_counter) << 8) | (*(c.cpu.program_counter + 1)); //Big endian
        printf("%04x ", instruction);

        uint8_t first_nibble = instruction >> 12;
        uint8_t second_nibble = (0x0F00 & instruction) >> 8;
        uint8_t third_nibble = (0x00F0 & instruction) >> 4;
        uint8_t fourth_nibble = 0x000F & instruction;

        printf("%.1x %.1x %.1x %.1x ", first_nibble, second_nibble, third_nibble, fourth_nibble);

        switch(first_nibble)
        {
            case 0x0:
                if(second_nibble == 0x0 && third_nibble == 0xE)
                {
                    if(fourth_nibble == 0x0)
                    {
                        printf("(Clear screen)\n");
                        SDL_RenderClear(c.display.renderer);
                    }
                    else if(fourth_nibble == 0xE)
                    {
                        printf("(Return)\n");
                    }
                }
                else
                {
                    printf("(Unknown)\n");
                }
                break;

            case 0x1:
                break;

            case 0x2:
                break;

            case 0x3:
                break;

            case 0x4:
                break;

            case 0x5:
                break;

            case 0x6:
                break;

            case 0x7:
                break;

            case 0x8:
                switch(fourth_nibble)
                {
                    case 0x0:
                        break;

                    case 0x1:
                        break;

                    case 0x2:
                        break;

                    case 0x3:
                        break;

                    case 0x4:
                        break;

                    case 0x5:
                        break;

                    case 0x6:
                        break;

                    case 0x7:
                        break;

                    case 0xE:
                        break;

                    default:
                        break;
                }

                break;

            case 0x9:
                break;

            case 0xA:
                break;

            case 0xB:
                break;

            case 0xC:
                break;

            case 0xD:
                break;

            case 0xE:
                if(third_nibble == 0x9 && fourth_nibble == 0xE)
                {

                }
                else if(third_nibble == 0xA && fourth_nibble == 0x1)
                {

                }
                break;

            case 0xF:
                if(third_nibble == 0x0)
                {
                    if(fourth_nibble == 0x7)
                    {

                    }
                    else if(fourth_nibble == 0xA)
                    {

                    }
                }
                else if(third_nibble == 0x1)
                {
                    if(fourth_nibble == 0x5)
                    {

                    }
                    else if(fourth_nibble == 0x8)
                    {

                    }
                    else if(fourth_nibble == 0xE)
                    {

                    }
                }
                else if(third_nibble == 0x2)
                {

                }
                else if(third_nibble == 0x3)
                {

                }
                else if(third_nibble == 0x5)
                {

                }
                else if(third_nibble == 0x6)
                {

                }
                break;

            default:
                printf("(Unknown for now)\n");
                break;
        }
        c.cpu.program_counter += 2;
    }
}


void destroy_chip_8(struct chip_8* c)
{
    SDL_DestroyRenderer(c->display.renderer);
    SDL_DestroyWindow(c->display.window);
}
