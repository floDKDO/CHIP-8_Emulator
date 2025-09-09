#ifndef __DISPLAY__
#define __DISPLAY__

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

#include "SDL2/SDL.h"

struct display
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Rect rect;
};


struct display create_display(void);

void destroy_display(struct display* d);



#endif // __DISPLAY__
