#pragma once

#include "sdl.hpp"
#include <optional>

struct Config;

class DisplayConfig
{
    std::optional<sdl::WindowPtr> window;
    std::optional<sdl::RendererPtr> renderer;
    int window_frame_scale = 1;
    int game_window_w = 0, game_window_h = 0;
    int intro_window_w = 0, intro_window_h = 0;

    void initWindowAndRenderer(int width, int height, Uint32 window_flags);
    void initFullMapMode(Config const &cfg, SDL_DisplayMode const &display_mode);
    void initNormalMode(Config const &cfg, SDL_DisplayMode const &display_mode);

public:
    explicit DisplayConfig(Config const &cfg) noexcept;
    SDL_Window *getWindow() const { return *window; }
    SDL_Renderer *getRenderer() const { return *renderer; }
    SDL_Rect getGameWindowRect() const;
    void setIntroViewport() const;
    void setGameViewport() const;
    std::pair<int, int> setWindowFrameViewport() const;
    std::pair<int, int> setScoreboardViewport() const;
};
