#pragma once

#include "sdl.hpp"
#include "Config.hpp"

struct DisplayConfig
{
    int w = 0, h = 0;
    DisplayConfig(Config const &cfg, SDL_Window *window, SDL_Renderer *renderer) noexcept;
};
