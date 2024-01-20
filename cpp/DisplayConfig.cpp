#include "DisplayConfig.hpp"
#include "Coord.hpp"

DisplayConfig::DisplayConfig(Config const &cfg,
                             SDL_Window *window, SDL_Renderer *renderer) noexcept
    : window(window)
    , renderer(renderer)
    , intro_window_w(cfg.logical_width)
    , intro_window_h(cfg.logical_height)
    , max_resolution(cfg.max_resolution)
{
    int window_w, window_h;
    SDL_GetWindowSize(window, &window_w, &window_h);

    if (cfg.max_resolution)
    {
        auto max_w = (cfg.map_width * MapPos::CELL_WIDTH + 20) * MapPos::PIXELS_PER_CHAR;
        auto max_h = (cfg.map_height * MapPos::CELL_HEIGHT + 15) * MapPos::PIXELS_PER_CHAR;
        game_window_w = std::min(max_w, window_w);
        game_window_h = std::min(max_h, window_h);
    }
    else
    {
        game_window_w = cfg.logical_width;
        game_window_h = cfg.logical_height;
    }
}

void DisplayConfig::setIntroViewport() const
{
    int window_w, window_h;
    SDL_GetWindowSize(window, &window_w, &window_h);

    auto gw_scale_w = static_cast<double>(window_w) / game_window_w;
    auto gw_scale_h = static_cast<double>(window_h) / game_window_h;
    auto gw_scale = std::min(gw_scale_w, gw_scale_h);
    auto iw_scale_w = static_cast<double>(game_window_w) / intro_window_w;
    auto iw_scale_h = static_cast<double>(game_window_h) / intro_window_h;
    auto iw_scale = std::min(iw_scale_w, iw_scale_h);
    auto scale = gw_scale * iw_scale;

    SDL_Rect rect{(static_cast<int>(window_w / scale) - intro_window_w) / 2,
                  (static_cast<int>(window_h / scale) - intro_window_h) / 2,
                  intro_window_w, intro_window_h};
    SDL_RenderSetScale(renderer, scale, scale);
    SDL_RenderSetViewport(renderer, &rect);
}

void DisplayConfig::setGameViewport() const
{
    int window_w, window_h;
    SDL_GetWindowSize(window, &window_w, &window_h);

    auto scale_w = static_cast<double>(window_w) / game_window_w;
    auto scale_h = static_cast<double>(window_h) / game_window_h;
    auto scale = std::min(scale_w, scale_h);

    SDL_Rect rect{(static_cast<int>(window_w / scale) - game_window_w) / 2,
                  (static_cast<int>(window_h / scale) - game_window_h) / 2,
                  game_window_w, game_window_h};
    SDL_RenderSetScale(renderer, scale, scale);
    SDL_RenderSetViewport(renderer, &rect);
}
