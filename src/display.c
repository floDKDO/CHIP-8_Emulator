#include "display.h"
#include "defs.h"

struct display create_display(void)
{
    struct display d;
    NCHK(d.window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN));
    NCHK(d.renderer = SDL_CreateRenderer(d.window, -1, SDL_RENDERER_PRESENTVSYNC));

    CHK(SDL_RenderSetLogicalSize(d.renderer, DISPLAY_WIDTH, DISPLAY_HEIGHT));
    d.rect.x = 0;
    d.rect.y = 0;
    d.rect.w = DISPLAY_WIDTH;
    d.rect.h = DISPLAY_HEIGHT;

    return d;
}

void destroy_display(struct display* d)
{
    SDL_DestroyRenderer(d->renderer);
    SDL_DestroyWindow(d->window);
}
