#include "DisplayConfig.hpp"
#include "Coord.hpp"
#include "Config.hpp"

std::pair<int, int> DisplayConfig::fullMapModeSize(Config const &cfg)
{
    // Size of the scoreboard and the window frame, in characters
    auto frame_x_size = (1 + Config::SCOREBOARD_WIDTH);
    auto frame_y_size = 1 + 1;
    // Map width in characters: the map width in cells, plus one extra cell on each side
    // of the map (so we show a block of sand on each side of the map)
    auto map_width_chars = (cfg.map_width + 2) * MapPos::CELL_WIDTH;
    // Map height in characters: the map height in cells, plus one extra cell on each side
    // of the map (so we show a block of sand on the bottom and a block of sky on top)
    auto map_height_chars = (cfg.map_height + 2) * MapPos::CELL_HEIGHT;
    return {(frame_x_size + map_width_chars) * MapPos::PIXELS_PER_CHAR,
            (frame_y_size + map_height_chars) * MapPos::PIXELS_PER_CHAR};
}

std::pair<sdl::WindowPtr, sdl::RendererPtr> DisplayConfig::initDisplay(Config const &cfg)
{
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    Uint32 window_flags = 0;
    int width = display_mode.w, height = display_mode.h;
    if (cfg.full_screen)
        window_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    else if (cfg.max_resolution)
    {
        auto [max_w, max_h] = fullMapModeSize(cfg);
        width = std::min(max_w, display_mode.w * 19 / 20);
        height = std::min(max_h, display_mode.h * 19 / 20);
        window_flags = SDL_WINDOW_RESIZABLE;
    }
    else
    {
        auto scale_w = static_cast<int>(static_cast<double>(display_mode.w * .8 / cfg.logical_width));
        auto scale_h = static_cast<int>(static_cast<double>(display_mode.h * .8 / cfg.logical_height));
        auto scale = std::max(std::min(scale_w, scale_h), 1);
        width = cfg.logical_width * scale;
        height = cfg.logical_height * scale;
        window_flags = SDL_WINDOW_RESIZABLE;
    }
    return sdl::createWindowAndRenderer(width, height, window_flags);
}



DisplayConfig::DisplayConfig(Config const &cfg) noexcept
    : w_and_r(initDisplay(cfg))
    , intro_window_w(cfg.logical_width)
    , intro_window_h(cfg.logical_height)
    , max_resolution(cfg.max_resolution)
{
    int window_w, window_h;
    SDL_GetWindowSize(getWindow(), &window_w, &window_h);

    if (cfg.max_resolution)
    {
        auto [max_w, max_h] = fullMapModeSize(cfg);
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
    SDL_GetWindowSize(getWindow(), &window_w, &window_h);

    auto gw_scale_w = static_cast<float>(window_w) / game_window_w;
    auto gw_scale_h = static_cast<float>(window_h) / game_window_h;
    auto gw_scale = std::min(gw_scale_w, gw_scale_h);
    auto iw_scale_w = static_cast<float>(game_window_w) / intro_window_w;
    auto iw_scale_h = static_cast<float>(game_window_h) / intro_window_h;
    auto iw_scale = std::min(iw_scale_w, iw_scale_h);
    auto scale = gw_scale * iw_scale;

    SDL_Rect rect{(static_cast<int>(window_w / scale) - intro_window_w) / 2,
                  (static_cast<int>(window_h / scale) - intro_window_h) / 2,
                  intro_window_w, intro_window_h};
    SDL_RenderSetScale(getRenderer(), scale, scale);
    SDL_RenderSetViewport(getRenderer(), &rect);
}

void DisplayConfig::setGameViewport() const
{
    int window_w, window_h;
    SDL_GetWindowSize(getWindow(), &window_w, &window_h);

    auto scale_w = static_cast<float>(window_w) / game_window_w;
    auto scale_h = static_cast<float>(window_h) / game_window_h;
    auto scale = std::min(scale_w, scale_h);

    SDL_Rect rect{(static_cast<int>(window_w / scale) - game_window_w) / 2,
                  (static_cast<int>(window_h / scale) - game_window_h) / 2,
                  game_window_w, game_window_h};
    SDL_RenderSetScale(getRenderer(), scale, scale);
    SDL_RenderSetViewport(getRenderer(), &rect);
}
