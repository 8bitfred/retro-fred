#include "DisplayConfig.hpp"
#include "Coord.hpp"

DisplayConfig::DisplayConfig(Config const &cfg,
                             SDL_Window *window, SDL_Renderer *renderer) noexcept
{
    int window_w, window_h;
    SDL_GetWindowSize(window, &window_w, &window_h);
    if (cfg.max_resolution)
    {
        auto max_w = (cfg.map_width * MapPos::CELL_WIDTH + 20) * MapPos::PIXELS_PER_CHAR;
        auto max_h = (cfg.map_height * MapPos::CELL_HEIGHT + 15) * MapPos::PIXELS_PER_CHAR;
        w = std::min(max_w, window_w);
        h = std::min(max_h, window_h);
    }
    else
    {
        w = cfg.logical_width;
        h = cfg.logical_height;
        SDL_RenderSetLogicalSize(renderer, w, h);
    }
}
