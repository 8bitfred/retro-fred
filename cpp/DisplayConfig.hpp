#pragma once

#include "sdl.hpp"
#include "Config.hpp"

class DisplayConfig
{
    SDL_Window *window;
    SDL_Renderer *renderer;
    int game_window_w = 0, game_window_h = 0;
    int intro_window_w = 0, intro_window_h = 0;
    bool max_resolution;

public:
    DisplayConfig(Config const &cfg, SDL_Window *window, SDL_Renderer *renderer) noexcept;
    int getGameWindowWidth() const { return game_window_w; }
    int getGameWindowHeight() const { return game_window_h; }
    void setIntroViewport() const;
    void setGameViewport() const;
};
