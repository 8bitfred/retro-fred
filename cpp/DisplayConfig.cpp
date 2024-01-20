#include "DisplayConfig.hpp"
#include "Coord.hpp"

DisplayConfig::DisplayConfig(Config const &cfg,
                             SDL_Window *window, SDL_Renderer *renderer) noexcept
    : window(window)
    , renderer(renderer)
    , logical_width(cfg.logical_width)
    , logical_height(cfg.logical_height)
    , max_resolution(cfg.max_resolution)
{
    int window_w, window_h;
    SDL_GetWindowSize(window, &window_w, &window_h);

    if (cfg.max_resolution)
    {
        auto max_w = (cfg.map_width * MapPos::CELL_WIDTH + 20) * MapPos::PIXELS_PER_CHAR;
        auto max_h = (cfg.map_height * MapPos::CELL_HEIGHT + 15) * MapPos::PIXELS_PER_CHAR;
        game_window_width = std::min(max_w, window_w);
        game_window_height = std::min(max_h, window_h);
    }
    else
    {
        game_window_width = cfg.logical_width;
        game_window_height = cfg.logical_height;
    }
}

void DisplayConfig::setIntroViewport() const
{
    int window_w, window_h;
    SDL_GetWindowSize(window, &window_w, &window_h);

    double scale_w = static_cast<double>(window_w) / logical_width;
    double scale_h = static_cast<double>(window_h) / logical_height;
    auto scale = std::min(scale_w, scale_h);
    SDL_Rect rect = {(static_cast<int>(window_w / scale) - logical_width) / 2,
                     (static_cast<int>(window_h / scale) - logical_height) / 2,
                     logical_width,
                     logical_height};

    SDL_RenderSetScale(renderer, scale, scale);
    SDL_RenderSetViewport(renderer, &rect);
}

void DisplayConfig::setGameViewport() const
{
    if (max_resolution)
    {
        SDL_RenderSetScale(renderer, 1, 1);
        SDL_RenderSetViewport(renderer, nullptr);
    }
    else
        setIntroViewport();
}
