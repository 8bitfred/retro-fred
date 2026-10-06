#pragma once

#include "platform/Renderer.hpp"
#include <memory>

class Platform;
struct Config;

class DisplayConfig
{
    std::unique_ptr<Renderer> renderer;
    int window_frame_scale = 1;
    int game_window_w = 0, game_window_h = 0;
    int intro_window_w = 0, intro_window_h = 0;

    void initWindowAndRenderer(Platform &platform, int width, int height, WindowFlags flags);
    void initFullMapMode(Platform &platform, Config const &cfg, std::pair<int, int> const &display_resolution);
    void initNormalMode(Platform &platform, Config const &cfg, std::pair<int, int> const &display_resolution);

public:
    DisplayConfig(Config const &cfg, Platform &platform);
    Renderer &getRenderer() const { return *renderer; }
    std::pair<int, int> getWindowSize() const;
    Rect getGameWindowRect() const;
    void setWindowTitleAndIcon(std::string const &title, Surface const &icon) const;
    void setIntroViewport() const;
    void setGameViewport() const;
    std::pair<int, int> setWindowFrameViewport() const;
    std::pair<int, int> setScoreboardViewport() const;
};
