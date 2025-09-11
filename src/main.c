#include "stack.h"
#include "chip_8.h"
#include "defs.h"
#include "SDL2/SDL_mixer.h"

#include <time.h>
#include <stdbool.h>

int main(void)
{
    CHK(SDL_Init(SDL_INIT_EVERYTHING));
    CHK(Mix_Init(MIX_INIT_OGG));
    CHK(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_CHANNELS, 1024));
    Mix_Chunk* beep_sound;
    beep_sound = Mix_LoadWAV("beep_sound.wav");

    struct chip_8 c = create_chip_8();
    //c = load_program_in_memory(c, "test_roms/IBM Logo.ch8");
    c = load_program_in_memory(c, "test_roms/3-corax+.ch8");
    //c = load_program_in_memory(c, "test_roms/1-chip8-logo.ch8");
    dump_memory(c);

    /*uint16_t valeur;
    struct stack s = create_stack();
    s = push_stack(s, 0x4423);
    s = pop_stack(s, &valeur);
    printf("Valeur : %d\n", valeur);*/

    srand(time(NULL));

    bool attendre = true;
    bool quit = false;
    while(!quit)
    {
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

                    switch(e.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            quit = true;
                            break;

                        case SDLK_1:
                            c.is_key_pressed[0x1] = true;
                            break;

                        case SDLK_2:
                            c.is_key_pressed[0x2] = true;
                            break;

                        case SDLK_3:
                            c.is_key_pressed[0x3] = true;
                            break;

                        case SDLK_4:
                            c.is_key_pressed[0xC] = true;
                            break;

                        case SDLK_a:
                            c.is_key_pressed[0x4] = true;
                            break;

                        case SDLK_z:
                            c.is_key_pressed[0x5] = true;
                            break;

                        case SDLK_e:
                            c.is_key_pressed[0x6] = true;
                            break;

                        case SDLK_r:
                            c.is_key_pressed[0xD] = true;
                            break;

                        case SDLK_q:
                            c.is_key_pressed[0x7] = true;
                            break;

                        case SDLK_s:
                            c.is_key_pressed[0x8] = true;
                            break;

                        case SDLK_d:
                            c.is_key_pressed[0x9] = true;
                            break;

                        case SDLK_f:
                            c.is_key_pressed[0xE] = true;
                            break;

                        case SDLK_w:
                            c.is_key_pressed[0xA] = true;
                            break;

                        case SDLK_x:
                            c.is_key_pressed[0x0] = true;
                            break;

                        case SDLK_c:
                            c.is_key_pressed[0xB] = true;
                            break;

                        case SDLK_v:
                            c.is_key_pressed[0xF] = true;
                            break;

                        case SDLK_RIGHT: //walking one instruction
                            attendre = false;
                            break;

                        default:
                            break;
                    }
                    break;

                case SDL_KEYUP :

                    switch(e.key.keysym.sym)
                    {
                        case SDLK_1:
                            c.is_key_pressed[0x1] = false;
                            break;

                        case SDLK_2:
                            c.is_key_pressed[0x2] = false;
                            break;

                        case SDLK_3:
                            c.is_key_pressed[0x3] = false;
                            break;

                        case SDLK_4:
                            c.is_key_pressed[0xC] = false;
                            break;

                        case SDLK_a:
                            c.is_key_pressed[0x4] = false;
                            break;

                        case SDLK_z:
                            c.is_key_pressed[0x5] = false;
                            break;

                        case SDLK_e:
                            c.is_key_pressed[0x6] = false;
                            break;

                        case SDLK_r:
                            c.is_key_pressed[0xD] = false;
                            break;

                        case SDLK_q:
                            c.is_key_pressed[0x7] = false;
                            break;

                        case SDLK_s:
                            c.is_key_pressed[0x8] = false;
                            break;

                        case SDLK_d:
                            c.is_key_pressed[0x9] = false;
                            break;

                        case SDLK_f:
                            c.is_key_pressed[0xE] = false;
                            break;

                        case SDLK_w:
                            c.is_key_pressed[0xA] = false;
                            break;

                        case SDLK_x:
                            c.is_key_pressed[0x0] = false;
                            break;

                        case SDLK_c:
                            c.is_key_pressed[0xB] = false;
                            break;

                        case SDLK_v:
                            c.is_key_pressed[0xF] = false;
                            break;

                        case SDLK_RIGHT: //walking one instruction
                            attendre = true;
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
        }

        if(c.cpu.sound_timer != 0)
        {
            Mix_PlayChannel(1, beep_sound, 0);
        }

        if(c.cpu.delay_timer > 0)
        {
           c.cpu.delay_timer -= 1;
        }

        //printf("LOOP\n");
        CHK(SDL_RenderClear(c.display.renderer));
        //color_specific_pixel(c, 63, 31, 0xFFFFFFFF);
        //printf("BOOL : %d\n", check_if_specific_pixel_on(c, 0, 0));

        //if(attendre == false)
            c = handle_instructions(c);
        attendre = true;

        CHK(SDL_RenderCopy(c.display.renderer, c.display.texture, NULL, NULL));
        SDL_RenderPresent(c.display.renderer);
        SDL_Delay(32);
    }

    destroy_chip_8(&c);
    Mix_FreeChunk(beep_sound);
    Mix_Quit();
    SDL_Quit();
	return EXIT_SUCCESS;
}
