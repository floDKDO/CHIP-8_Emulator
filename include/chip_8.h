#ifndef __PROCESSING__
#define __PROCESSING__

#define RAM_SIZE 4096
#define START_ADDRESS 512

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 512

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

#include <stdbool.h>
#include "SDL2/SDL.h"
#include "stack.h"


struct display
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Rect rect;
    SDL_Texture* texture;
};


struct cpu
{
    uint16_t program_counter;
    uint16_t index_register;
    uint8_t v_registers[16];
    uint8_t delay_timer;
    uint8_t sound_timer;
};


struct memory
{
    uint8_t memory_array[RAM_SIZE];
};


struct chip_8
{
    struct display display;
    struct cpu cpu;
    struct memory memory;
    struct stack stack;
    bool is_key_pressed[16];
};


struct chip_8 create_chip_8(void);
struct chip_8 clear_all_pixels(struct chip_8 c);
struct chip_8 color_specific_pixel(struct chip_8 c, int x, int y, Uint32 color);
bool check_if_specific_pixel_on(struct chip_8 c, int x, int y);
bool check_if_any_key_pressed(struct chip_8 c);
int get_pressed_key(struct chip_8 c);
struct chip_8 load_program_in_memory(struct chip_8 c, char* pathname);
void dump_memory(struct chip_8 c);
struct chip_8 handle_instructions(struct chip_8 c);
void destroy_chip_8(struct chip_8* c);



#endif // __PROCESSING__
