#pragma once

#include "sdl.hpp"

class TextureManager;

class Controller
{
public:
    static std::pair<SDL_Rect, SDL_Rect> getRects(SDL_Window *window);
    static void render(SDL_Window *window, SDL_Renderer *renderer,
                       TextureManager const &tmgr);
};
