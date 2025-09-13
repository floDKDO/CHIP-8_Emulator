#include "chip_8.h"

struct chip_8 create_chip_8(void)
{
    struct chip_8 c;

    NCHK(c.display.window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE), SDL_GetError());
    snprintf(c.display.window_title, 16, "%s", "CHIP-8 Emulator");
    NCHK(c.display.renderer = SDL_CreateRenderer(c.display.window, -1, SDL_RENDERER_ACCELERATED), SDL_GetError());
    CHK(SDL_RenderSetLogicalSize(c.display.renderer, DISPLAY_WIDTH, DISPLAY_HEIGHT), SDL_GetError());
    NCHK(c.display.texture = SDL_CreateTexture(c.display.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, DISPLAY_WIDTH, DISPLAY_HEIGHT), SDL_GetError());

    memset(c.memory.memory_array, 0, RAM_SIZE * sizeof(uint8_t));

    uint8_t font[80] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    memcpy(c.memory.memory_array + START_ADDRESS_FONT, font, sizeof(font));

    c.cpu.program_counter = START_ADDRESS_ROM;
    c.cpu.stack_pointer = 0;
    c.cpu.index_register = 0;
    memset(c.cpu.v_registers, 0, 16 * sizeof(uint8_t));
    c.cpu.delay_timer = 0;
    c.cpu.sound_timer = 0;

    memset(c.stack.addresses, 0, MAX_STACK_SIZE * sizeof(uint16_t));

    memset(c.is_key_pressed, false, 16 * sizeof(bool));
    c.key_already_pressed = false;

    c.number_of_instructions_executed = 0;
    c.is_draw_instruction = false;

    return c;
}


struct chip_8 push_stack(struct chip_8 c, uint16_t address)
{
    c.stack.addresses[c.cpu.stack_pointer] = c.cpu.program_counter; //First push PC to the stack
    c.cpu.program_counter = address; //and then set PC to NNN (= address here)
    c.cpu.stack_pointer += 1;
    return c;
}


struct chip_8 pop_stack(struct chip_8 c)
{
    c.cpu.stack_pointer -= 1;
    c.cpu.program_counter = c.stack.addresses[c.cpu.stack_pointer];
    return c;
}


struct chip_8 clear_all_pixels(struct chip_8 c)
{
    void* pixels;
    int pitch;

    CHK(SDL_LockTexture(c.display.texture, NULL, &pixels, &pitch), SDL_GetError());

    Uint32* pixelsPtr = ((Uint32*)pixels);
    memset(pixelsPtr, 0x000000FF, pitch * DISPLAY_HEIGHT);

    SDL_UnlockTexture(c.display.texture);

    return c;
}



struct chip_8 color_specific_pixel(struct chip_8 c, int x, int y, Uint32 color)
{
    void* pixels;
    int pitch;

    CHK(SDL_LockTexture(c.display.texture, NULL, &pixels, &pitch), SDL_GetError());

    Uint32* pixelsPtr = ((Uint32*)pixels);
    int index = y * (pitch / sizeof(Uint32)) + x;
    pixelsPtr[index] = color;

    SDL_UnlockTexture(c.display.texture);

    return c;
}


bool check_if_specific_pixel_is_on(struct chip_8 c, int x, int y)
{
    void* pixels;
    int pitch;

    CHK(SDL_LockTexture(c.display.texture, NULL, &pixels, &pitch), SDL_GetError());

    Uint32* pixelsPtr = ((Uint32*)pixels);
    int index = y * (pitch / sizeof(Uint32)) + x; //(pitch / sizeof(Uint32)) = DISPLAY_WIDTH
    bool is_present = (pixelsPtr[index] != 0x00000000 && pixelsPtr[index] != 0x000000FF); //<=> black pixel

    SDL_UnlockTexture(c.display.texture);

    return is_present;
}


int get_pressed_key(struct chip_8 c)
{
    for(int i = 0; i < 16; ++i)
    {
        if(c.is_key_pressed[i])
        {
            return i;
        }
    }
    return -1;
}


struct chip_8 load_program_in_memory(struct chip_8 c, char* pathname)
{
    if((c.memory.rom = fopen(pathname, "r")) == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    size_t s_read;
    if((s_read = fread(c.memory.memory_array + START_ADDRESS_ROM, sizeof(char), RAM_SIZE, c.memory.rom)) == 0)
    {
        fprintf(stderr, "fread: error!\n");
        exit(EXIT_FAILURE);
    }

    if(ferror(c.memory.rom))
    {
        fprintf(stderr, "ferror: error!\n");
        exit(EXIT_FAILURE);
    }

    return c;
}

void dump_memory(struct chip_8 c)
{
    for(int i = 0; i < RAM_SIZE; ++i)
    {
        if(i == START_ADDRESS_ROM)
        {
            printf("###START OF PROGRAM###\n");
        }
        printf("%d: %02x\n", i, c.memory.memory_array[i]);
    }
}


struct chip_8 handle_instructions(struct chip_8 c, int num_instr_per_frame)
{
    DEBUG_PRINT("PC : %d\n", c.cpu.program_counter);

    for(int i = 0; i < num_instr_per_frame; ++i)
    {
        uint16_t instruction = ((c.memory.memory_array[c.cpu.program_counter] << 8) | (c.memory.memory_array[c.cpu.program_counter + 1])); //Big endian

        uint8_t first_nibble = instruction >> 12;
        uint8_t second_nibble = (0x0F00 & instruction) >> 8;
        uint8_t third_nibble = (0x00F0 & instruction) >> 4;
        uint8_t fourth_nibble = 0x000F & instruction;

        if(instruction != 0)
        {
            DEBUG_PRINT("%04x ", instruction);
            DEBUG_PRINT("%.1x %.1x %.1x %.1x ", first_nibble, second_nibble, third_nibble, fourth_nibble);
        }

        bool program_counter_edited = false;

        switch(first_nibble)
        {
            case 0x0:
                if(second_nibble == 0x0 && third_nibble == 0xE)
                {
                    if(fourth_nibble == 0x0)
                    {
                        DEBUG_PRINT("(Clear screen)\n");
                        c.is_draw_instruction = true;
                        clear_all_pixels(c);
                    }
                    else if(fourth_nibble == 0xE)
                    {
                        DEBUG_PRINT("(Return) ");
                        DEBUG_PRINT("Value of SP: %d ", c.cpu.stack_pointer);
                        c = pop_stack(c);
                        DEBUG_PRINT("Return address: %d\n", c.cpu.program_counter);
                    }
                }
                break;

            case 0x1:
                DEBUG_PRINT("(Jump) ");
                {
                    uint16_t address = second_nibble << 8 | third_nibble << 4 | fourth_nibble;
                    DEBUG_PRINT("Address: %04x\n", address);
                    c.cpu.program_counter = address;
                    program_counter_edited = true;
                }
                break;

            case 0x2:
                DEBUG_PRINT("(Jump) ");
                {
                    uint16_t address = second_nibble << 8 | third_nibble << 4 | fourth_nibble;
                    DEBUG_PRINT("Address: %04x\n", address);
                    c = push_stack(c, address);
                    program_counter_edited = true;
                }
                break;

            case 0x3:
                DEBUG_PRINT("(Skip if VX == NN) ");
                {
                    uint8_t value_in_vx = c.cpu.v_registers[second_nibble];
                    uint8_t nn = third_nibble << 4 | fourth_nibble;
                    DEBUG_PRINT("Value of V%d: %d, value of NN: %d", second_nibble, value_in_vx, nn);
                    if(value_in_vx == nn)
                    {
                        c.cpu.program_counter += 2;
                        DEBUG_PRINT(" -> Skip\n");
                    }
                    else
                    {
                        DEBUG_PRINT(" -> No skip\n");
                    }
                }
                break;

            case 0x4:
                DEBUG_PRINT("(Skip if VX != NN) ");
                {
                    uint8_t value_in_vx = c.cpu.v_registers[second_nibble];
                    uint8_t nn = third_nibble << 4 | fourth_nibble;
                    DEBUG_PRINT("Value of V%d: %d, value of NN: %d", second_nibble, value_in_vx, nn);
                    if(value_in_vx != nn)
                    {
                        c.cpu.program_counter += 2;
                        DEBUG_PRINT(" -> Skip\n");
                    }
                    else
                    {
                        DEBUG_PRINT(" -> No skip\n");
                    }
                }
                break;

            case 0x5:
                DEBUG_PRINT("(Skip if VX == VY) ");
                {
                    uint8_t value_in_vx = c.cpu.v_registers[second_nibble];
                    uint8_t value_in_vy = c.cpu.v_registers[third_nibble];
                    DEBUG_PRINT("Value of V%d: %d, value of V%d: %d", second_nibble, value_in_vx, third_nibble, value_in_vy);
                    if(value_in_vx == value_in_vy)
                    {
                        c.cpu.program_counter += 2;
                        DEBUG_PRINT(" -> Skip\n");
                    }
                    else
                    {
                        DEBUG_PRINT(" -> No skip\n");
                    }
                }
                break;

            case 0x6:
                {
                    uint8_t value = third_nibble << 4 | fourth_nibble;
                    uint8_t index = second_nibble;
                    c.cpu.v_registers[index] = value;
                    DEBUG_PRINT("(Put value %d in V%d register)\n", value, index);
                }
                break;

            case 0x7:
                {
                    uint8_t value = third_nibble << 4 | fourth_nibble;
                    uint8_t index = second_nibble;
                    c.cpu.v_registers[index] += value;
                    DEBUG_PRINT("(Add value %d in V%d register)\n", value, index);
                }
                break;

            case 0x8:
                switch(fourth_nibble)
                {
                    case 0x0:
                        DEBUG_PRINT("(Set VX to VY)\n");
                        {
                            uint8_t value_in_vy = c.cpu.v_registers[third_nibble];
                            c.cpu.v_registers[second_nibble] = value_in_vy;
                        }
                        break;

                    case 0x1:
                        DEBUG_PRINT("(Set VX to VX | VY)\n");
                        {
                            uint8_t value_in_vy = c.cpu.v_registers[third_nibble];
                            c.cpu.v_registers[second_nibble] |= value_in_vy;
                            if(c.mode == LEGACY)
                            {
                                c.cpu.v_registers[0xF] = 0;
                            }
                        }
                        break;

                    case 0x2:
                        DEBUG_PRINT("(Set VX to VX & VY)\n");
                        {
                            uint8_t value_in_vy = c.cpu.v_registers[third_nibble];
                            c.cpu.v_registers[second_nibble] &= value_in_vy;
                            if(c.mode == LEGACY)
                            {
                                c.cpu.v_registers[0xF] = 0;
                            }
                        }
                        break;

                    case 0x3:
                        DEBUG_PRINT("(Set VX to VX ^ VY)\n");
                        {
                            uint8_t value_in_vy = c.cpu.v_registers[third_nibble];
                            c.cpu.v_registers[second_nibble] ^= value_in_vy;
                            if(c.mode == LEGACY)
                            {
                                c.cpu.v_registers[0xF] = 0;
                            }
                        }
                        break;

                    case 0x4:
                        DEBUG_PRINT("(Set VX to VX + VY)\n");
                        {
                            uint8_t value_in_vx = c.cpu.v_registers[second_nibble];
                            uint8_t value_in_vy = c.cpu.v_registers[third_nibble];
                            c.cpu.v_registers[second_nibble] += value_in_vy;
                            if(value_in_vx + value_in_vy > 255)
                            {
                                c.cpu.v_registers[0xF] = 1;
                            }
                            else c.cpu.v_registers[0xF] = 0;
                        }
                        break;

                    case 0x5:
                        DEBUG_PRINT("(Set VX to VX - VY)\n");
                        {
                            uint8_t value_in_vx = c.cpu.v_registers[second_nibble];
                            uint8_t value_in_vy = c.cpu.v_registers[third_nibble];
                            c.cpu.v_registers[second_nibble] -= value_in_vy;
                            if(value_in_vx >= value_in_vy)
                            {
                                c.cpu.v_registers[0xF] = 1;
                            }
                            else if(value_in_vy > value_in_vx)
                            {
                                c.cpu.v_registers[0xF] = 0;
                            }
                        }
                        break;

                    case 0x6:
                        DEBUG_PRINT("(Set VX to VX >> VY)\n");
                        {
                            if(c.mode == LEGACY)
                            {
                                c.cpu.v_registers[second_nibble] = c.cpu.v_registers[third_nibble]; //VX = VY
                            }
                            uint16_t vx_before_modif = c.cpu.v_registers[second_nibble];
                            c.cpu.v_registers[second_nibble] >>= 1;
                            if((vx_before_modif & 0x01) == 0)
                            {
                                c.cpu.v_registers[0xF] = 0;
                            }
                            else
                            {
                                c.cpu.v_registers[0xF] = 1;
                            }
                        }
                        break;

                    case 0x7:
                        DEBUG_PRINT("(Set VX to VY - VX)\n");
                        {
                            uint8_t value_in_vx = c.cpu.v_registers[second_nibble];
                            uint8_t value_in_vy = c.cpu.v_registers[third_nibble];
                            c.cpu.v_registers[second_nibble] = value_in_vy - value_in_vx;
                            if(value_in_vy >= value_in_vx)
                            {
                                c.cpu.v_registers[0xF] = 1;
                            }
                            else
                            {
                                c.cpu.v_registers[0xF] = 0;
                            }
                        }
                        break;

                    case 0xE:
                        DEBUG_PRINT("(Set VX to VX << VY)\n");
                        {
                            if(c.mode == LEGACY)
                            {
                                c.cpu.v_registers[second_nibble] = c.cpu.v_registers[third_nibble]; //VX = VY
                            }
                            uint16_t vx_before_modif = c.cpu.v_registers[second_nibble];
                            c.cpu.v_registers[second_nibble] <<= 1;
                            if((vx_before_modif & 0x80) == 0)
                            {
                                c.cpu.v_registers[0xF] = 0;
                            }
                            else
                            {
                                c.cpu.v_registers[0xF] = 1;
                            }
                        }
                        break;

                    default:
                        break;
                }

                break;

            case 0x9:
                DEBUG_PRINT("(Skip if VX != VY) ");
                {
                    uint8_t value_in_vx = c.cpu.v_registers[second_nibble];
                    uint8_t value_in_vy = c.cpu.v_registers[third_nibble];
                    DEBUG_PRINT("Value of V%d: %d, value of V%d: %d", second_nibble, value_in_vx, third_nibble, value_in_vy);
                    if(value_in_vx != value_in_vy)
                    {
                        c.cpu.program_counter += 2;
                        DEBUG_PRINT(" -> Skip\n");
                    }
                    else
                    {
                        DEBUG_PRINT(" -> No skip\n");
                    }
                }
                break;

            case 0xA:
                {
                    uint16_t value = second_nibble << 8 | third_nibble << 4 | fourth_nibble;
                    c.cpu.index_register = value; //value and not START_ADDRESS_ROM + value
                    DEBUG_PRINT("(Store address %d in index register)\n", value);
                }
                break;

            case 0xB:
                DEBUG_PRINT("(Jump with offset) ");
                {
                    uint16_t address = second_nibble << 8 | third_nibble << 4 | fourth_nibble;
                    DEBUG_PRINT("Address: %04x\n", address);
                    if(c.mode == LEGACY)
                    {
                        uint8_t value_in_v0 = c.cpu.v_registers[0];
                        c = push_stack(c, address + value_in_v0);
                    }
                    else if(c.mode == MODERN)
                    {
                        int8_t value_in_vx = c.cpu.v_registers[second_nibble];
                        c = push_stack(c, address + value_in_vx);
                    }
                    program_counter_edited = true;
                }
                break;

            case 0xC:
                DEBUG_PRINT("(Generate random number)\n");
                {
                    int r = rand() % 256;
                    uint8_t nn = third_nibble << 4 | fourth_nibble;
                    c.cpu.v_registers[second_nibble] = r & nn;
                }
                break;

            case 0xD:
                DEBUG_PRINT("(Display)\n");
                {
                    c.is_draw_instruction = true;

                    uint8_t register_x = second_nibble;
                    uint8_t register_y = third_nibble;

                    int x_pos = c.cpu.v_registers[register_x] & (DISPLAY_WIDTH - 1);
                    int y_pos = c.cpu.v_registers[register_y] & (DISPLAY_HEIGHT - 1);

                    c.cpu.v_registers[0xF] = 0;

                    uint8_t n = fourth_nibble;

                    for(uint8_t y = 0; y < n; ++y)
                    {
                        uint8_t nth_byte = c.memory.memory_array[c.cpu.index_register + y];

                        for(uint8_t x = 0; x < 8; ++x)
                        {
                            int pixel_x = (x_pos + x) % DISPLAY_WIDTH;
                            int pixel_y = (y_pos + y) % DISPLAY_HEIGHT;

                            if(x_pos + x > DISPLAY_WIDTH - 1)
                            {
                                break;
                            }

                            if(nth_byte & (0x80 >> x))
                            {
                                if(check_if_specific_pixel_is_on(c, pixel_x, pixel_y))
                                {
                                    color_specific_pixel(c, pixel_x, pixel_y, 0x000000FF);
                                    c.cpu.v_registers[0xF] = 1;
                                }
                                else color_specific_pixel(c, pixel_x, pixel_y, 0xFFFFFFFF);
                            }
                        }

                        if(y_pos + y > DISPLAY_HEIGHT - 1)
                        {
                            break;
                        }
                    }
                }
                break;

            case 0xE:
                if(third_nibble == 0x9 && fourth_nibble == 0xE)
                {
                    DEBUG_PRINT("(Skip instruction if key pressed)\n");
                    uint8_t value_in_vx = c.cpu.v_registers[second_nibble];
                    if(c.is_key_pressed[value_in_vx])
                    {
                        c.cpu.program_counter += 2;
                    }
                }
                else if(third_nibble == 0xA && fourth_nibble == 0x1)
                {
                    DEBUG_PRINT("(Skip instruction if key is not pressed)\n");
                    uint8_t value_in_vx = c.cpu.v_registers[second_nibble];
                    if(!c.is_key_pressed[value_in_vx])
                    {
                        c.cpu.program_counter += 2;
                    }
                }
                break;

            case 0xF:
                if(third_nibble == 0x0)
                {
                    if(fourth_nibble == 0x7)
                    {
                        DEBUG_PRINT("(Set VX to the value in delay timer)\n");
                        c.cpu.v_registers[second_nibble] = c.cpu.delay_timer;
                    }
                    else if(fourth_nibble == 0xA)
                    {
                        DEBUG_PRINT("(Wait for user input)\n");

                        if((c.index_last_pressed_key = get_pressed_key(c)) == -1 && !c.key_already_pressed)
                        {
                            c.cpu.program_counter -= 2;
                        }
                        else if((c.index_last_pressed_key = get_pressed_key(c)) != -1 && !c.key_already_pressed)
                        {
                            c.key_already_pressed = true;
                        }

                        if(c.key_already_pressed)
                        {
                            if(get_pressed_key(c) != -1)
                            {
                                c.cpu.program_counter -= 2;
                            }
                            else
                            {
                                int index = c.index_last_pressed_key;
                                c.cpu.v_registers[second_nibble] = index;
                                c.index_last_pressed_key = -1;
                                c.key_already_pressed = false;
                            }
                        }
                    }
                }
                else if(third_nibble == 0x1)
                {
                    if(fourth_nibble == 0x5)
                    {
                        DEBUG_PRINT("(Set delay timer to the value in VX)\n");
                        c.cpu.delay_timer = c.cpu.v_registers[second_nibble];
                    }
                    else if(fourth_nibble == 0x8)
                    {
                        DEBUG_PRINT("(Set sound timer to the value in VX)\n");
                        c.cpu.sound_timer = c.cpu.v_registers[second_nibble];
                    }
                    else if(fourth_nibble == 0xE)
                    {
                        DEBUG_PRINT("(Set Index register to Index register + VX)\n");
                        c.cpu.index_register += c.cpu.v_registers[second_nibble];
                    }
                }
                else if(third_nibble == 0x2)
                {
                    DEBUG_PRINT("(Set Index register to the address in VX)\n");
                    c.cpu.index_register = START_ADDRESS_FONT + (c.cpu.v_registers[second_nibble] * 5); // * 5 because there are 5 bytes per letter
                }
                else if(third_nibble == 0x3)
                {
                    DEBUG_PRINT("(Convert byte number to three decimal digits)\n");
                    uint8_t value_in_vx = c.cpu.v_registers[second_nibble];
                    c.memory.memory_array[c.cpu.index_register] = value_in_vx / 100;
                    c.memory.memory_array[c.cpu.index_register + 1] = (value_in_vx % 100) / 10;
                    c.memory.memory_array[c.cpu.index_register + 2] = (value_in_vx % 100) % 10;
                }
                else if(third_nibble == 0x5)
                {
                    DEBUG_PRINT("(Save all V registers value in memory beginning at the Index register)\n");
                    uint8_t x = second_nibble;
                    for(uint16_t i = 0; i <= x; ++i)
                    {
                        if(c.mode == LEGACY)
                        {
                            c.memory.memory_array[c.cpu.index_register] = c.cpu.v_registers[i];
                            c.cpu.index_register += 1;
                        }
                        else if(c.mode == MODERN)
                        {
                            c.memory.memory_array[c.cpu.index_register + i] = c.cpu.v_registers[i];
                        }
                    }
                }
                else if(third_nibble == 0x6)
                {
                    DEBUG_PRINT("(Load in all V registers the values that are in memory beginning at the Index register) ");
                    uint8_t x = second_nibble;
                    for(uint16_t i = 0; i <= x; ++i)
                    {
                        if(c.mode == LEGACY)
                        {
                            c.cpu.v_registers[i] = c.memory.memory_array[c.cpu.index_register];
                            c.cpu.index_register += 1;
                        }
                        else if(c.mode == MODERN)
                        {
                            c.cpu.v_registers[i] = c.memory.memory_array[c.cpu.index_register + i];
                        }
                        DEBUG_PRINT("V%d value: %d", i, c.cpu.v_registers[i]);
                    }
                    DEBUG_PRINT("\n");
                }
                break;

            default:
                DEBUG_PRINT("(Unknown instruction (%d)\n", instruction);
                break;
        }

        if(!program_counter_edited)
        {
            c.cpu.program_counter += 2;
        }

        c.number_of_instructions_executed += 1;
    }
    return c;
}


void destroy_chip_8(struct chip_8* c)
{
    fclose(c->memory.rom);
    SDL_DestroyTexture(c->display.texture);
    SDL_DestroyRenderer(c->display.renderer);
    SDL_DestroyWindow(c->display.window);
}
