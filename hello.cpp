#include "sdl.hpp"
 
// for rendering images and graphics on screen
#include <SDL2/SDL_image.h>
 
// for using SDL_Delay() functions
#include <SDL2/SDL_timer.h>

int main(int argc, char *argv[])
{
  sdl::app my_app;
 
  auto [window, renderer] = sdl::create_window_and_renderer(5*256, 5*192);
  SDL_RenderSetScale(renderer, 5, 5);

  sdl::surface surface(IMG_Load("sprites/fred_1x_01.png"));
  SDL_SetColorKey(surface, SDL_TRUE, 0);
  sdl::texture texture(SDL_CreateTextureFromSurface(renderer, surface));
  //sdl::texture texture(IMG_LoadTexture(renderer, "sprites/fred_1x_01c.png"));
  while (1) {
    SDL_Event event;
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT)
      break;

    SDL_Rect dest{14*8, 160, 24, 32};
    SDL_SetRenderDrawColor(renderer, 0x30, 0x40, 0x50, 0x10);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, &dest);
    SDL_Rect dest2{14*8+6, 160, 24, 32};
    SDL_RenderCopy(renderer, texture, nullptr, &dest2);
    SDL_RenderPresent(renderer);        
  }
 
    return 0;
}
