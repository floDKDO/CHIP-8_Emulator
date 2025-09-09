#include "display.h"
#include "stack.h"
#include "cpu.h"
#include "defs.h"

#include <stdbool.h>

int main(void)
{
    CHK(SDL_Init(SDL_INIT_EVERYTHING));

    struct memory m = create_memory();
    m = load_program_in_memory(m, "IBM Logo.ch8");
    dump_memory(m);
    read_instructions(m);

    struct stack s = create_stack();

    struct display d = create_display();

    bool quitter = false;
    while(!quitter)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            switch(e.type)
            {
                case SDL_WINDOWEVENT :

                    if(e.window.event == SDL_WINDOWEVENT_CLOSE)
                    {
                        quitter = true;
                    }
                    break;

                case SDL_KEYDOWN :

                    switch(e.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            quitter = true;
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

        CHK(SDL_RenderClear(d.renderer));
        CHK(SDL_SetRenderDrawColor(d.renderer, 255, 0, 0, 0));
        SDL_RenderPresent(d.renderer);
    }

    destroy_display(&d);
    SDL_Quit();
	return EXIT_SUCCESS;
}
