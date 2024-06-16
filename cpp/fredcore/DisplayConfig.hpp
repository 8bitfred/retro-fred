#pragma once

#include "sdl.hpp"

struct Config;

class DisplayConfig
{
    std::pair<sdl::WindowPtr, sdl::RendererPtr> w_and_r;
    int game_window_w = 0, game_window_h = 0;
    int intro_window_w = 0, intro_window_h = 0;
    bool max_resolution;

    static std::pair<int, int> fullMapModeSize(Config const &cfg);
    static std::pair<sdl::WindowPtr, sdl::RendererPtr> initDisplay(Config const &cfg);

public:
    explicit DisplayConfig(Config const &cfg) noexcept;
    SDL_Window *getWindow() const { return w_and_r.first; }
    SDL_Renderer *getRenderer() const { return w_and_r.second; }
    int getGameWindowWidth() const { return game_window_w; }
    int getGameWindowHeight() const { return game_window_h; }
    void setIntroViewport() const;
    void setGameViewport() const;
};
