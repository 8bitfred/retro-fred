#include "DisplayConfig.hpp"
#include "Coord.hpp"
#include "Config.hpp"

void DisplayConfig::initWindowAndRenderer(int width, int height, Uint32 window_flags)
{
    auto [win, ren] = sdl::createWindowAndRenderer(width, height, window_flags);
    window.emplace(std::move(win));
    renderer.emplace(std::move(ren));
}

void DisplayConfig::initFullMapMode(Config const &cfg, SDL_DisplayMode const &display_mode)
{
    // Scale of the window frame (and scoreboard)
    auto scale_w = static_cast<int>(static_cast<double>(display_mode.w * .6 / cfg.logical_width));
    auto scale_h = static_cast<int>(static_cast<double>(display_mode.h * .6 / cfg.logical_height));
    window_frame_scale = std::max(std::min(scale_w, scale_h), 1);

    // Size of the scoreboard and the window frame, in characters
    auto frame_x_size = (1 + Config::SCOREBOARD_WIDTH) * MapPos::PIXELS_PER_CHAR;
    auto frame_y_size = (1 + 1) * MapPos::PIXELS_PER_CHAR;
    // Map width in characters: the map width in cells, plus one extra cell on each side
    // of the map (so we show a block of sand on each side of the map)
    auto map_width_pixels = (cfg.map_width + 2) * MapPos::CELL_WIDTH_PIXELS;
    // Map height in characters: the map height in cells, plus one extra cell on each side
    // of the map (so we show a block of sand on the bottom and a block of sky on top)
    auto map_height_pixels = (cfg.map_height + 2) * MapPos::CELL_HEIGHT_PIXELS;
    auto max_w = frame_x_size * window_frame_scale + map_width_pixels;
    auto max_h = frame_y_size * window_frame_scale + map_height_pixels;

    Uint32 window_flags = cfg.high_dpi ? SDL_WINDOW_ALLOW_HIGHDPI : 0;
    if (cfg.user_window_size)
        initWindowAndRenderer(cfg.window_width, cfg.window_height,
                              window_flags | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);
    else
    {
        int width = std::min(max_w, display_mode.w * 19 / 20);
        int height = std::min(max_h, display_mode.h * 19 / 20);
        initWindowAndRenderer(width, height, window_flags | SDL_WINDOW_RESIZABLE);
    }

    auto [window_w, window_h] = getWindowSize();

    game_window_w = std::min(max_w, window_w);
    game_window_h = std::min(max_h, window_h);
}

void DisplayConfig::initNormalMode(Config const &cfg, SDL_DisplayMode const &display_mode)
{
    Uint32 window_flags = cfg.high_dpi ? SDL_WINDOW_ALLOW_HIGHDPI : 0;
    int width = display_mode.w, height = display_mode.h;
    if (cfg.full_screen)
        window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    else if (cfg.user_window_size)
    {
        width = cfg.window_width;
        height = cfg.window_height;
        window_flags |= SDL_WINDOW_BORDERLESS;
    }
    else
    {
        auto scale_w = static_cast<int>(static_cast<double>(display_mode.w * .8 / cfg.logical_width));
        auto scale_h = static_cast<int>(static_cast<double>(display_mode.h * .8 / cfg.logical_height));
        auto scale = std::max(std::min(scale_w, scale_h), 1);
        width = cfg.logical_width * scale;
        height = cfg.logical_height * scale;
        window_flags |= SDL_WINDOW_RESIZABLE;
    }
    initWindowAndRenderer(width, height, window_flags);
    game_window_w = cfg.logical_width;
    game_window_h = cfg.logical_height;
}

DisplayConfig::DisplayConfig(Config const &cfg) noexcept
    : intro_window_w(cfg.logical_width)
    , intro_window_h(cfg.logical_height)
{
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    if (cfg.max_resolution)
        initFullMapMode(cfg, display_mode);
    else
        initNormalMode(cfg, display_mode);
}

std::pair<int, int> DisplayConfig::getWindowSize() const
{
    int window_w, window_h;
    SDL_GetRendererOutputSize(getRenderer(), &window_w, &window_h);
    return {window_w, window_h};
}

SDL_Rect DisplayConfig::getGameWindowRect() const
{
    return {
        1 * MapPos::PIXELS_PER_CHAR * window_frame_scale,
        1 * MapPos::PIXELS_PER_CHAR * window_frame_scale,
        game_window_w - ((1+Config::SCOREBOARD_WIDTH) * MapPos::PIXELS_PER_CHAR * window_frame_scale),
        game_window_h - (2 * MapPos::PIXELS_PER_CHAR * window_frame_scale)};
}

void DisplayConfig::setIntroViewport() const
{
    auto [window_w, window_h] = getWindowSize();
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
    auto [window_w, window_h] = getWindowSize();
    auto scale_w = static_cast<float>(window_w) / game_window_w;
    auto scale_h = static_cast<float>(window_h) / game_window_h;
    auto scale = std::min(scale_w, scale_h);

    SDL_RenderSetScale(getRenderer(), 1, 1);
    SDL_Rect rect{(window_w - static_cast<int>(game_window_w * scale)) / 2,
                  (window_h - static_cast<int>(game_window_h * scale)) / 2,
                  static_cast<int>(game_window_w * scale),
                  static_cast<int>(game_window_h * scale)};
    SDL_RenderSetViewport(getRenderer(), &rect);
    SDL_RenderSetScale(getRenderer(), scale, scale);
}

std::pair<int, int> DisplayConfig::setWindowFrameViewport() const
{
    auto [window_w, window_h] = getWindowSize();
    auto scale_w = static_cast<float>(window_w) / game_window_w;
    auto scale_h = static_cast<float>(window_h) / game_window_h;
    auto scale = std::min(scale_w, scale_h);

    auto scoreboard_w = Config::SCOREBOARD_WIDTH * MapPos::PIXELS_PER_CHAR * window_frame_scale;
    auto viewport_w = static_cast<int>((game_window_w - scoreboard_w) * scale);
    SDL_RenderSetScale(getRenderer(), 1, 1);
    SDL_Rect rect{(window_w - static_cast<int>(game_window_w * scale)) / 2,
                  (window_h - static_cast<int>(game_window_h * scale)) / 2,
                  viewport_w,
                  static_cast<int>(game_window_h * scale)};
    SDL_RenderSetViewport(getRenderer(), &rect);
    SDL_RenderSetScale(getRenderer(), scale*window_frame_scale, scale*window_frame_scale);

    return {(game_window_w + window_frame_scale - 1) / window_frame_scale,
            (game_window_h + window_frame_scale - 1) / window_frame_scale};
}

std::pair<int, int> DisplayConfig::setScoreboardViewport() const
{
    auto [window_w, window_h] = getWindowSize();
    auto scale_w = static_cast<float>(window_w) / game_window_w;
    auto scale_h = static_cast<float>(window_h) / game_window_h;
    auto scale = std::min(scale_w, scale_h);

    auto scoreboard_w = Config::SCOREBOARD_WIDTH * MapPos::PIXELS_PER_CHAR * window_frame_scale;
    auto frame_x = (window_w - static_cast<int>(game_window_w * scale)) / 2;
    auto scoreboard_x = frame_x + static_cast<int>((game_window_w - scoreboard_w) * scale);
    SDL_RenderSetScale(getRenderer(), 1, 1);
    SDL_Rect rect{scoreboard_x,
                  (window_h - static_cast<int>(game_window_h * scale)) / 2,
                  static_cast<int>(scoreboard_w * scale),
                  static_cast<int>(game_window_h * scale)};
    SDL_RenderSetViewport(getRenderer(), &rect);
    SDL_RenderSetScale(getRenderer(), scale*window_frame_scale, scale*window_frame_scale);

    return {scoreboard_w / window_frame_scale,
            (game_window_h + window_frame_scale - 1) / window_frame_scale};
}
