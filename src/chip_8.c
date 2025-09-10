#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "defs.h"
#include "chip_8.h"


struct chip_8 create_chip_8(void)
{
    struct chip_8 c;

    NCHK(c.display.window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE));
    NCHK(c.display.renderer = SDL_CreateRenderer(c.display.window, -1, SDL_RENDERER_PRESENTVSYNC));
    CHK(SDL_RenderSetLogicalSize(c.display.renderer, DISPLAY_WIDTH, DISPLAY_HEIGHT));
    c.display.rect.x = 0;
    c.display.rect.y = 0;
    c.display.rect.w = DISPLAY_WIDTH;
    c.display.rect.h = DISPLAY_HEIGHT;
    NCHK(c.display.texture = SDL_CreateTexture(c.display.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, DISPLAY_WIDTH, DISPLAY_HEIGHT));

    memset(c.memory.memory_array, 0, RAM_SIZE * sizeof(char));

    c.cpu.program_counter = &(c.memory.memory_array[START_ADDRESS]);
    c.cpu.index_register = NULL;
    memset(c.cpu.v_registers, 0, 16 * sizeof(unsigned char));

    c.stack = create_stack();

    return c;
}


struct chip_8 color_specific_pixel(struct chip_8 c, int x, int y, Uint32 color)
{
    void* pixels;
    int pitch;

    CHK(SDL_LockTexture(c.display.texture, NULL, &pixels, &pitch));

    Uint32* pixelsPtr = ((Uint32*)pixels);

    int index = y * (pitch / sizeof(Uint32)) + x;
    pixelsPtr[index] = color;

    SDL_UnlockTexture(c.display.texture);

    return c;
}


bool check_if_specific_pixel_on(struct chip_8 c, int x, int y)
{
    void* pixels;
    int pitch;

    CHK(SDL_LockTexture(c.display.texture, NULL, &pixels, &pitch));

    Uint32* pixelsPtr = ((Uint32*)pixels);

    int index = y * (pitch / sizeof(Uint32)) + x;

    bool is_present = (pixelsPtr[index] != 0x00000000 && pixelsPtr[index] != 0xFF000000);

    printf("IIISSSS PPPRREEESSSEEENNNTTT à l'index %d : %d\n", index, is_present);

    printf("Valeur du pixel : %u\n", pixelsPtr[index]);

    SDL_UnlockTexture(c.display.texture);

    return is_present;
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


//Le programme en mémoire va de l'index 512 à 643
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

        bool program_counter_edited = false;

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
                {
                    uint16_t address = second_nibble << 8 | third_nibble << 4 | fourth_nibble;
                    //printf("Address : %04x, ", address);
                    c.cpu.program_counter = &(c.memory.memory_array[START_ADDRESS + address]);
                    //printf("Adresse de PC : %p, adresse case : %p\n", c.cpu.program_counter, &(c.memory.memory_array[START_ADDRESS]));
                    program_counter_edited = true;
                }
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
                {
                    uint8_t value = third_nibble << 4 | fourth_nibble;
                    uint8_t index = second_nibble;
                    c.cpu.v_registers[index] = value;
                    printf("Valeur stockée dans V%d : %d\n", index, value);
                }
                break;

            case 0x7:
                {
                    uint8_t value = third_nibble << 4 | fourth_nibble;
                    uint8_t index = second_nibble;
                    c.cpu.v_registers[index] += value;
                    printf("Valeur additionnée au contenu du registre V%d : %d\n", index, value);
                }
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
                {
                    uint16_t value = second_nibble << 8 | third_nibble << 4 | fourth_nibble;
                    printf("VVVAAALLLEEEUUURRR : %u\n", value);
                    c.cpu.index_register = &(c.memory.memory_array[value]); //[value] and not [START_ADDRESS + value]

                    printf("Adresse stockée dans I : %p\n", c.cpu.index_register);
                }
                break;

            case 0xB:
                break;

            case 0xC:
                break;

            case 0xD:

                printf("DISPLAY\n");
                {
                    uint8_t register_x = second_nibble;
                    uint8_t register_y = third_nibble;

                    int x_pos = c.cpu.v_registers[register_x] & 63;
                    int y_pos = c.cpu.v_registers[register_y] & 31;

                    printf("xpos : %d, ypos : %d\n", x_pos, y_pos);

                    c.cpu.v_registers[0xF] = 0;

                    uint8_t n = fourth_nibble;
                    for(uint8_t y = 0; y < n; ++y)
                    {
                        uint8_t nth_byte = c.cpu.index_register[y];
                        printf("NTH Byte : %u\n", nth_byte);

                        for(int x = 0; x < 8; ++x)
                        {
                            //printf("Valeur avec masque : %u\n", nth_byte & mask);
                            if(nth_byte & (0x80 >> x))
                            {
                                printf("Un !\n");
                                if(check_if_specific_pixel_on(c, x_pos, y_pos))
                                {
                                    printf("LA! \n");
                                    color_specific_pixel(c, x_pos, y_pos, 0xFF000000);
                                    c.cpu.v_registers[0xF] = 1;
                                }
                                else
                                {
                                    printf("ICI! \n");
                                    color_specific_pixel(c, x_pos, y_pos, 0xFFFFFFFF);
                                    c.cpu.v_registers[0xF] = 0;
                                }
                            }
                            else
                            {
                                printf("Zéro ! \n");
                            }

                            printf("XPOS : %d\n", x_pos);

                            if(x_pos > DISPLAY_WIDTH)
                                break;

                            x_pos += 1;
                        }
                        y_pos += 1;
                    }
                }
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
        if(!program_counter_edited)
        {
            c.cpu.program_counter += 2;
        }
    }
}


void destroy_chip_8(struct chip_8* c)
{
    SDL_DestroyTexture(c->display.texture);
    SDL_DestroyRenderer(c->display.renderer);
    SDL_DestroyWindow(c->display.window);
}
