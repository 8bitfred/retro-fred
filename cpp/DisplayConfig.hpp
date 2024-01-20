#pragma once

#include "sdl.hpp"
#include "Config.hpp"

class DisplayConfig
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    int game_window_width = 0, game_window_height = 0;
    int logical_width = 0, logical_height = 0;
    bool max_resolution;

public:
    DisplayConfig(Config const &cfg, SDL_Window *window, SDL_Renderer *renderer) noexcept;
    int getGameWindowWidth() const { return game_window_width; }
    int getGameWindowHeight() const { return game_window_height; }
    void setIntroViewport() const;
    void setGameViewport() const;
};
