#include "stack.h"
#include "chip_8.h"
#include "defs.h"

#include <stdbool.h>

int main(void)
{
    CHK(SDL_Init(SDL_INIT_EVERYTHING));

    struct chip_8 c = create_chip_8();
    c = load_program_in_memory(c, "IBM Logo.ch8");
    dump_memory(c);

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
                            break;

                        case SDLK_2:
                            break;

                        case SDLK_3:
                            break;

                        case SDLK_4:
                            break;


                        case SDLK_a:
                            break;

                        case SDLK_z:
                            break;

                        case SDLK_e:
                            break;

                        case SDLK_r:
                            break;


                        case SDLK_q:
                            break;

                        case SDLK_s:
                            break;

                        case SDLK_d:
                            break;

                        case SDLK_f:
                            break;


                        case SDLK_w:
                            break;

                        case SDLK_x:
                            break;

                        case SDLK_c:
                            break;

                        case SDLK_v:
                            break;

                        default:
                            break;
                    }

                default:
                    break;
            }
        }

        CHK(SDL_RenderClear(c.display.renderer));
        //color_specific_pixel(c, 63, 31, 0xFFFFFFFF);
        //printf("BOOL : %d\n", check_if_specific_pixel_on(c, 0, 0));
        handle_instructions(c);
        CHK(SDL_RenderCopy(c.display.renderer, c.display.texture, NULL, NULL));
        SDL_RenderPresent(c.display.renderer);
        SDL_Delay(32);
    }

    destroy_chip_8(&c);
    SDL_Quit();
	return EXIT_SUCCESS;
}
