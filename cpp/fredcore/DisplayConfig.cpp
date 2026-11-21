#include "DisplayConfig.hpp"
#include "platform/Platform.hpp"
#include "Coord.hpp"
#include "Config.hpp"

void DisplayConfig::initWindowAndRenderer(Platform &platform, int width, int height,
                                          WindowFlags flags)
{
    renderer = platform.createWindowAndRenderer(width, height, flags);
}

void DisplayConfig::initFullMapMode(Platform &platform, Config const &cfg,
                                    std::pair<int, int> const &display_resolution)
{
    auto [display_w, display_h] = display_resolution;
    // Scale of the window frame (and scoreboard)
    auto scale_w = static_cast<int>(static_cast<double>(display_w * .6 / cfg.logical_width));
    auto scale_h = static_cast<int>(static_cast<double>(display_h * .6 / cfg.logical_height));
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

    WindowFlags window_flags = cfg.high_dpi ? WindowFlags::HIGH_DPI : WindowFlags::NONE;
    if (cfg.user_window_size)
        initWindowAndRenderer(platform, cfg.window_width, cfg.window_height,
                              window_flags | WindowFlags::RESIZABLE | WindowFlags::BORDERLESS);
    else
    {
        int width = std::min(max_w, display_w * 19 / 20);
        int height = std::min(max_h, display_h * 19 / 20);
        initWindowAndRenderer(platform, width, height, window_flags | WindowFlags::RESIZABLE);
    }

    auto [window_w, window_h] = getWindowSize();

    game_window_w = std::min(max_w, window_w);
    game_window_h = std::min(max_h, window_h);
}

void DisplayConfig::initNormalMode(Platform &platform, Config const &cfg,
                                   std::pair<int, int> const &display_resolution)
{
    auto [display_w, display_h] = display_resolution;
    WindowFlags window_flags = cfg.high_dpi ? WindowFlags::HIGH_DPI : WindowFlags::NONE;
    int width = display_w, height = display_h;
    if (cfg.full_screen)
        window_flags = window_flags | WindowFlags::FULLSCREEN_DESKTOP;
    else if (cfg.user_window_size)
    {
        width = cfg.window_width;
        height = cfg.window_height;
        window_flags = window_flags | WindowFlags::BORDERLESS;
    }
    else
    {
        auto scale_w = static_cast<int>(static_cast<double>(display_w * .8 / cfg.logical_width));
        auto scale_h = static_cast<int>(static_cast<double>(display_h * .8 / cfg.logical_height));
        auto scale = std::max(std::min(scale_w, scale_h), 1);
        width = cfg.logical_width * scale;
        height = cfg.logical_height * scale;
        window_flags = window_flags | WindowFlags::RESIZABLE;
    }
    initWindowAndRenderer(platform, width, height, window_flags);
    game_window_w = cfg.logical_width;
    game_window_h = cfg.logical_height;
}

DisplayConfig::DisplayConfig(Config const &cfg, Platform &platform)
    : intro_window_w(cfg.logical_width)
    , intro_window_h(cfg.logical_height)
{
    auto display_resolution = platform.getDisplayResolution();
    if (cfg.max_resolution)
        initFullMapMode(platform, cfg, display_resolution);
    else
        initNormalMode(platform, cfg, display_resolution);
}

std::pair<int, int> DisplayConfig::getWindowSize() const
{
    return renderer->getOutputSize();
}

Rect DisplayConfig::getGameWindowRect() const
{
    return {
        1 * MapPos::PIXELS_PER_CHAR * window_frame_scale,
        1 * MapPos::PIXELS_PER_CHAR * window_frame_scale,
        game_window_w - ((1+Config::SCOREBOARD_WIDTH) * MapPos::PIXELS_PER_CHAR * window_frame_scale),
        game_window_h - (2 * MapPos::PIXELS_PER_CHAR * window_frame_scale)};
}

void DisplayConfig::setWindowTitleAndIcon(std::string const &title, Surface const &icon) const
{
    renderer->setTitle(title);
    renderer->setIcon(icon);
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

    // The viewport is in raw pixels (independent of the scale): center the intro area, whose
    // size is intro_window_w x intro_window_h in scaled units, in the window.
    auto pixel_w = static_cast<int>(intro_window_w * scale);
    auto pixel_h = static_cast<int>(intro_window_h * scale);
    Rect rect{(window_w - pixel_w) / 2, (window_h - pixel_h) / 2, pixel_w, pixel_h};
    renderer->setScale(scale, scale);
    renderer->setViewport(&rect);
}

void DisplayConfig::setGameViewport() const
{
    auto [window_w, window_h] = getWindowSize();
    auto scale_w = static_cast<float>(window_w) / game_window_w;
    auto scale_h = static_cast<float>(window_h) / game_window_h;
    auto scale = std::min(scale_w, scale_h);

    renderer->setScale(1, 1);
    Rect rect{(window_w - static_cast<int>(game_window_w * scale)) / 2,
             (window_h - static_cast<int>(game_window_h * scale)) / 2,
             static_cast<int>(game_window_w * scale),
             static_cast<int>(game_window_h * scale)};
    renderer->setViewport(&rect);
    renderer->setScale(scale, scale);
}

std::pair<int, int> DisplayConfig::setWindowFrameViewport() const
{
    auto [window_w, window_h] = getWindowSize();
    auto scale_w = static_cast<float>(window_w) / game_window_w;
    auto scale_h = static_cast<float>(window_h) / game_window_h;
    auto scale = std::min(scale_w, scale_h);

    auto scoreboard_w = Config::SCOREBOARD_WIDTH * MapPos::PIXELS_PER_CHAR * window_frame_scale;
    auto viewport_w = static_cast<int>((game_window_w - scoreboard_w) * scale);
    renderer->setScale(1, 1);
    Rect rect{(window_w - static_cast<int>(game_window_w * scale)) / 2,
             (window_h - static_cast<int>(game_window_h * scale)) / 2,
             viewport_w,
             static_cast<int>(game_window_h * scale)};
    renderer->setViewport(&rect);
    renderer->setScale(scale*window_frame_scale, scale*window_frame_scale);

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
    renderer->setScale(1, 1);
    Rect rect{scoreboard_x,
             (window_h - static_cast<int>(game_window_h * scale)) / 2,
             static_cast<int>(scoreboard_w * scale),
             static_cast<int>(game_window_h * scale)};
    renderer->setViewport(&rect);
    renderer->setScale(scale*window_frame_scale, scale*window_frame_scale);

    return {scoreboard_w / window_frame_scale,
            (game_window_h + window_frame_scale - 1) / window_frame_scale};
}
