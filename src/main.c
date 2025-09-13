#include <time.h>
#include <errno.h>
#include <limits.h>

#include "chip_8.h"
#include "SDL2/SDL_mixer.h"

int main(int argc, char* argv[])
{
    srand(time(NULL));

    CHK(SDL_Init(SDL_INIT_EVERYTHING), SDL_GetError());
    CHK(Mix_Init(MIX_INIT_OGG), Mix_GetError());
    CHK(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_CHANNELS, 1024), Mix_GetError());

    Mix_Chunk* beep_sound;
    NCHK(beep_sound = Mix_LoadWAV("beep_sound.wav"), Mix_GetError());

    struct chip_8 c = create_chip_8();

    if(argc != 2)
    {
        c.mode = LEGACY; //legacy behavior by default
    }
    else
    {
        errno = 0;
        char* endptr;
        long int mode = strtol(argv[1], &endptr, 10);
        if((errno == EINVAL && mode == 0) || (errno == ERANGE && (mode == LONG_MIN || mode == LONG_MAX)))
        {
            fprintf(stderr, "strtol: error!\n");
            exit(EXIT_FAILURE);
        }
        if(mode != 0 && mode != 1)
        {
            fprintf(stderr, "Usage: ./CHIP-8 Emulator [0=LEGACY,default ; 1=MODERN]");
            exit(EXIT_FAILURE);
        }
        c.mode = ((int)mode);
    }

    /*choose the rom by uncommenting the desired line*/
    //c = load_program_in_memory(c, "test_roms/1-chip8-logo.ch8");
    //c = load_program_in_memory(c, "test_roms/2-ibm-logo.ch8");
    //c = load_program_in_memory(c, "test_roms/3-corax+.ch8");
    //c = load_program_in_memory(c, "test_roms/4-flags.ch8");
    //c = load_program_in_memory(c, "test_roms/5-quirks.ch8");
    //c = load_program_in_memory(c, "test_roms/6-keypad.ch8");
    //c = load_program_in_memory(c, "test_roms/7-beep.ch8");
    c = load_program_in_memory(c, "test_roms/TETRIS.ch8");

    #ifdef DEBUG
        dump_memory(c);
    #endif

	Uint32 actual_time, time_since_last_frame, delta_time;

    #ifdef DEBUG
        bool wait = true;
    #endif

    Uint32 begin_time, end_time;

    bool quit = false;
    while(!quit)
    {
        actual_time = SDL_GetTicks();
        delta_time = actual_time - time_since_last_frame;

        begin_time = SDL_GetTicks();

        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_WINDOWEVENT :

                    if(e.window.event == SDL_WINDOWEVENT_CLOSE)
                    {
                        quit = true;
                    }
                    break;

                case SDL_KEYDOWN :
                    switch(e.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_ESCAPE:
                            quit = true;
                            break;

                        case SDL_SCANCODE_1:
                            c.is_key_pressed[0x1] = true;
                            break;

                        case SDL_SCANCODE_2:
                            c.is_key_pressed[0x2] = true;
                            break;

                        case SDL_SCANCODE_3:
                            c.is_key_pressed[0x3] = true;
                            break;

                        case SDL_SCANCODE_4:
                            c.is_key_pressed[0xC] = true;
                            break;

                        case SDL_SCANCODE_Q:
                            c.is_key_pressed[0x4] = true;
                            break;

                        case SDL_SCANCODE_W:
                            c.is_key_pressed[0x5] = true;
                            break;

                        case SDL_SCANCODE_E:
                            c.is_key_pressed[0x6] = true;
                            break;

                        case SDL_SCANCODE_R:
                            c.is_key_pressed[0xD] = true;
                            break;

                        case SDL_SCANCODE_A:
                            c.is_key_pressed[0x7] = true;
                            break;

                        case SDL_SCANCODE_S:
                            c.is_key_pressed[0x8] = true;
                            break;

                        case SDL_SCANCODE_D:
                            c.is_key_pressed[0x9] = true;
                            break;

                        case SDL_SCANCODE_F:
                            c.is_key_pressed[0xE] = true;
                            break;

                        case SDL_SCANCODE_Z:
                            c.is_key_pressed[0xA] = true;
                            break;

                        case SDL_SCANCODE_X:
                            c.is_key_pressed[0x0] = true;
                            break;

                        case SDL_SCANCODE_C:
                            c.is_key_pressed[0xB] = true;
                            break;

                        case SDL_SCANCODE_V:
                            c.is_key_pressed[0xF] = true;
                            break;

                        case SDL_SCANCODE_RIGHT: //stepping one instruction
                            #ifdef DEBUG
                                wait = false;
                            #endif
                            break;

                        default:
                            break;
                    }
                    break;

                case SDL_KEYUP :

                    c.index_last_pressed_key = -1;
                    switch(e.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_1:
                            c.is_key_pressed[0x1] = false;
                            break;

                        case SDL_SCANCODE_2:
                            c.is_key_pressed[0x2] = false;
                            break;

                        case SDL_SCANCODE_3:
                            c.is_key_pressed[0x3] = false;
                            break;

                        case SDL_SCANCODE_4:
                            c.is_key_pressed[0xC] = false;
                            break;

                        case SDL_SCANCODE_Q:
                            c.is_key_pressed[0x4] = false;
                            break;

                        case SDL_SCANCODE_W:
                            c.is_key_pressed[0x5] = false;
                            break;

                        case SDL_SCANCODE_E:
                            c.is_key_pressed[0x6] = false;
                            break;

                        case SDL_SCANCODE_R:
                            c.is_key_pressed[0xD] = false;
                            break;

                        case SDL_SCANCODE_A:
                            c.is_key_pressed[0x7] = false;
                            break;

                        case SDL_SCANCODE_S:
                            c.is_key_pressed[0x8] = false;
                            break;

                        case SDL_SCANCODE_D:
                            c.is_key_pressed[0x9] = false;
                            break;

                        case SDL_SCANCODE_F:
                            c.is_key_pressed[0xE] = false;
                            break;

                        case SDL_SCANCODE_Z:
                            c.is_key_pressed[0xA] = false;
                            break;

                        case SDL_SCANCODE_X:
                            c.is_key_pressed[0x0] = false;
                            break;

                        case SDL_SCANCODE_C:
                            c.is_key_pressed[0xB] = false;
                            break;

                        case SDL_SCANCODE_V:
                            c.is_key_pressed[0xF] = false;
                            break;

                        case SDL_SCANCODE_RIGHT: //stepping one instruction
                            #ifdef DEBUG
                                wait = true;
                            #endif
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
        }

        DEBUG_PRINT("Number of instructions executed : %d\n", c.number_of_instructions_executed);

        if(c.cpu.sound_timer != 0)
        {
            CHK(Mix_PlayChannel(1, beep_sound, 0), Mix_GetError());
        }

        #ifdef DEBUG
        if(!wait)
        {
            c = handle_instructions(c, 1);
            wait = true;
        }
        #else
            c = handle_instructions(c, 11); //60FPS and 11 instructions per frame = approximately 700 CHIP-8 instructions per second
        #endif

        end_time = SDL_GetTicks();
        if(end_time - begin_time <= 16) //limit to 60 FPS
		{
			SDL_Delay(16 - (end_time - begin_time));

			time_since_last_frame = actual_time;

            char fps[10];
            snprintf(fps, sizeof(fps), "(%d FPS)", 1000 / delta_time);
            snprintf(c.display.window_title + 15, sizeof(c.display.window_title) - 15, " %s", fps); //15 is the length of the "CHIP-8 Emulator" string
            SDL_SetWindowTitle(c.display.window, c.display.window_title);

            if(c.is_draw_instruction)
            {
                CHK(SDL_RenderClear(c.display.renderer), SDL_GetError());
                CHK(SDL_RenderCopy(c.display.renderer, c.display.texture, NULL, NULL), SDL_GetError());
                SDL_RenderPresent(c.display.renderer);
                c.is_draw_instruction = false;
            }

            if(c.cpu.delay_timer > 0)
            {
               c.cpu.delay_timer -= 1;
            }

            if(c.cpu.sound_timer > 0)
            {
               c.cpu.sound_timer -= 1;
            }
        }
    }

    destroy_chip_8(&c);
    Mix_FreeChunk(beep_sound);
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
	return EXIT_SUCCESS;
}
