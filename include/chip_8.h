#ifndef __CHIP_8__
#define __CHIP_8__

//#define DEBUG //uncomment this line to display debugging information and have the ability to step the program instruction by instruction

#ifdef DEBUG
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#define NCHK(op, type) do { if ((op) == NULL){fprintf(stderr, "%s\n", type); exit(EXIT_FAILURE);} }while(0)
#define CHK(op, type) do { if ((op) < 0){fprintf(stderr, "%s\n", type); exit(EXIT_FAILURE);} }while(0)


#define RAM_SIZE 4096
#define START_ADDRESS_ROM 512
#define START_ADDRESS_FONT 80

#define MAX_STACK_SIZE 1024

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

#define WINDOW_MULTIPLICATION_FACTOR 16

#define WINDOW_WIDTH DISPLAY_WIDTH * WINDOW_MULTIPLICATION_FACTOR
#define WINDOW_HEIGHT DISPLAY_HEIGHT * WINDOW_MULTIPLICATION_FACTOR

#define FPS 60


#include <stdbool.h>
#include "SDL2/SDL.h"

struct display
{
    SDL_Window* window;
    char window_title[32];
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};


struct cpu
{
    uint16_t program_counter;
    uint16_t stack_pointer;
    uint16_t index_register;
    uint8_t v_registers[16];
    uint8_t delay_timer;
    uint8_t sound_timer;
};


struct memory
{
    uint8_t memory_array[RAM_SIZE];
    FILE* rom;
};

struct stack
{
    uint16_t addresses[MAX_STACK_SIZE];
};


struct chip_8
{
    enum modes{LEGACY, MODERN} mode; //0 for LEGACY and 1 for MODERN behavior

    struct display display;
    struct cpu cpu;
    struct memory memory;
    struct stack stack;
    bool is_key_pressed[16];
    int index_last_pressed_key;
    bool key_already_pressed;

    int number_of_instructions_executed;
    bool is_draw_instruction;
};


struct chip_8 create_chip_8(void);

struct chip_8 push_stack(struct chip_8 c, uint16_t address);
struct chip_8 pop_stack(struct chip_8 c);

struct chip_8 clear_all_pixels(struct chip_8 c);
struct chip_8 color_specific_pixel(struct chip_8 c, int x, int y, Uint32 color);
bool check_if_specific_pixel_is_on(struct chip_8 c, int x, int y);

int get_pressed_key(struct chip_8 c);

struct chip_8 load_program_in_memory(struct chip_8 c, char* pathname);
void dump_memory(struct chip_8 c);

struct chip_8 handle_instructions(struct chip_8 c, int num_instr_per_frame);

void destroy_chip_8(struct chip_8* c);



#endif // __CHIP_8__
