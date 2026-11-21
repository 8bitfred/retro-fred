#include "DisplayConfig.hpp"
#include "platform/Platform.hpp"
#include "Coord.hpp"
#include "Config.hpp"
#include "Controller.hpp"
#include <algorithm>
#include <cmath>
#include <tuple>

namespace
{
    // Maximum width of the game window, in game pixels, in full map mode when the size of
    // the window is fixed (e.g. a phone screen), in portrait and landscape orientations.
    // The window is scaled up to fill the screen, so this determines how much of the map is
    // visible.
    constexpr int FULL_MAP_MAX_WIDTH_PORTRAIT = 512;
    constexpr int FULL_MAP_MAX_WIDTH_LANDSCAPE = 1024;
}

void DisplayConfig::initWindowAndRenderer(Platform &platform, int width, int height,
                                          WindowFlags flags)
{
    renderer = platform.createWindowAndRenderer(width, height, flags);
}

// Size of a window with the whole map, with the current scale of the window frame
std::pair<int, int> DisplayConfig::getFullMapSize(Config const &cfg) const
{
    // Size of the scoreboard and the window frame, in characters
    auto frame_x_size = (1 + Config::SCOREBOARD_WIDTH) * MapPos::PIXELS_PER_CHAR;
    auto frame_y_size = (1 + 1) * MapPos::PIXELS_PER_CHAR;
    // Map width in characters: the map width in cells, plus one extra cell on each side
    // of the map (so we show a block of sand on each side of the map)
    auto map_width_pixels = (cfg.map_width + 2) * MapPos::CELL_WIDTH_PIXELS;
    // Map height in characters: the map height in cells, plus one extra cell on each side
    // of the map (so we show a block of sand on the bottom and a block of sky on top)
    auto map_height_pixels = (cfg.map_height + 2) * MapPos::CELL_HEIGHT_PIXELS;
    return {frame_x_size * window_frame_scale + map_width_pixels,
            frame_y_size * window_frame_scale + map_height_pixels};
}

// Scale of the window frame (and scoreboard) in full map mode with a window that can have
// any size: the frame must be legible when the map is shown at scale 1
int DisplayConfig::getFullMapFrameScale(Config const &cfg) const
{
    auto [display_w, display_h] = display_resolution;
    auto scale_w = static_cast<int>(static_cast<double>(display_w * .6 / cfg.logical_width));
    auto scale_h = static_cast<int>(static_cast<double>(display_h * .6 / cfg.logical_height));
    return std::max(std::min(scale_w, scale_h), 1);
}

std::pair<int, int> DisplayConfig::getFullMapWindowSize(Config const &cfg) const
{
    auto [display_w, display_h] = display_resolution;
    auto [max_w, max_h] = getFullMapSize(cfg);
    return {std::min(max_w, display_w * 19 / 20), std::min(max_h, display_h * 19 / 20)};
}

std::pair<int, int> DisplayConfig::getNormalWindowSize(Config const &cfg) const
{
    auto [display_w, display_h] = display_resolution;
    auto scale_w = static_cast<int>(static_cast<double>(display_w * .8 / cfg.logical_width));
    auto scale_h = static_cast<int>(static_cast<double>(display_h * .8 / cfg.logical_height));
    auto scale = std::max(std::min(scale_w, scale_h), 1);
    return {cfg.logical_width * scale, cfg.logical_height * scale};
}

// Layout of the screen in full map mode, for a window that can have any size: the map is
// shown at scale 1, and the window frame is scaled up
void DisplayConfig::setFullMapLayout(Config const &cfg)
{
    window_frame_scale = getFullMapFrameScale(cfg);
    auto [max_w, max_h] = getFullMapSize(cfg);
    auto [window_w, window_h] = getWindowSize();
    game_window_w = std::min(max_w, window_w);
    game_window_h = std::min(max_h, window_h);
    reserved_side = reserved_bottom = 0;
}

// Layout of the screen in normal mode: the original screen, scaled to fit the window
void DisplayConfig::setNormalLayout(Config const &cfg)
{
    game_window_w = cfg.logical_width;
    game_window_h = cfg.logical_height;
    window_frame_scale = 1;
    reserved_side = reserved_bottom = 0;
}

void DisplayConfig::initFullMapMode(Platform &platform, Config const &cfg)
{
    // The size of the window depends on the scale of the window frame
    window_frame_scale = getFullMapFrameScale(cfg);

    WindowFlags window_flags = cfg.high_dpi ? WindowFlags::HIGH_DPI : WindowFlags::NONE;
    if (cfg.user_window_size)
        initWindowAndRenderer(platform, cfg.window_width, cfg.window_height,
                              window_flags | WindowFlags::RESIZABLE | WindowFlags::BORDERLESS);
    else
    {
        auto [width, height] = getFullMapWindowSize(cfg);
        initWindowAndRenderer(platform, width, height, window_flags | WindowFlags::RESIZABLE);
    }
    setFullMapLayout(cfg);
}

void DisplayConfig::initNormalMode(Platform &platform, Config const &cfg)
{
    auto [display_w, display_h] = display_resolution;
    WindowFlags window_flags = cfg.high_dpi ? WindowFlags::HIGH_DPI : WindowFlags::NONE;
    int width = display_w, height = display_h;
    if (cfg.full_screen)
        // Also mark the window as resizable: on Android, SDL only allows the device to be
        // rotated if the window is resizable (otherwise it locks the orientation the app
        // was started in).
        window_flags = window_flags | WindowFlags::FULLSCREEN_DESKTOP | WindowFlags::RESIZABLE;
    else if (cfg.user_window_size)
    {
        width = cfg.window_width;
        height = cfg.window_height;
        window_flags = window_flags | WindowFlags::BORDERLESS;
    }
    else
    {
        std::tie(width, height) = getNormalWindowSize(cfg);
        window_flags = window_flags | WindowFlags::RESIZABLE;
    }
    initWindowAndRenderer(platform, width, height, window_flags);
    setNormalLayout(cfg);
}

DisplayConfig::DisplayConfig(Config const &cfg, Platform &platform)
    : display_resolution(platform.getDisplayResolution())
    , fixed_window(cfg.fixed_window_size)
    , resizable_window(!cfg.fixed_window_size && !cfg.full_screen && !cfg.user_window_size)
    , full_map_active(cfg.max_resolution && !cfg.fixed_window_size)
    , intro_window_w(cfg.logical_width)
    , intro_window_h(cfg.logical_height)
{
    if (full_map_active)
        initFullMapMode(platform, cfg);
    else
        initNormalMode(platform, cfg);
}

std::pair<int, int> DisplayConfig::getWindowSize() const
{
    return renderer->getOutputSize();
}

void DisplayConfig::updateLayout(Config const &cfg)
{
    if (fixed_window)
    {
        setFixedWindowLayout(cfg);
        return;
    }

    // The full map mode can be changed while the game is running: resize the window to fit
    // the screen of the new mode
    if (cfg.max_resolution == full_map_active)
        return;
    full_map_active = cfg.max_resolution;
    if (full_map_active)
    {
        window_frame_scale = getFullMapFrameScale(cfg);
        if (resizable_window)
        {
            auto [width, height] = getFullMapWindowSize(cfg);
            renderer->setWindowSize(width, height);
        }
        setFullMapLayout(cfg);
    }
    else
    {
        if (resizable_window)
        {
            auto [width, height] = getNormalWindowSize(cfg);
            renderer->setWindowSize(width, height);
        }
        setNormalLayout(cfg);
    }
}

// Layout of the screen for a window of fixed size (e.g. a phone screen). In full map mode
// the game window is scaled up to fill the window, so we limit its width to limit the scale
// and make the map visible. The size of the window can change when the device is rotated.
void DisplayConfig::setFixedWindowLayout(Config const &cfg)
{
    if (!cfg.max_resolution)
    {
        setNormalLayout(cfg);
        return;
    }

    auto [window_w, window_h] = getWindowSize();
    auto max_game_w = window_w < window_h ? FULL_MAP_MAX_WIDTH_PORTRAIT
                                          : FULL_MAP_MAX_WIDTH_LANDSCAPE;

    // Leave room for the virtual controller, so that it only covers part of the game
    auto margins = cfg.virtual_controller ? Controller::getMargins(window_w, window_h)
                                          : Controller::Margins{};
    reserved_side = margins.side;
    reserved_bottom = margins.bottom;
    auto area_w = window_w - 2 * reserved_side;
    auto area_h = window_h - reserved_bottom;

    // Scale the window frame and the scoreboard so that they are about the same size on
    // the screen as in normal mode.
    auto normal_scale = std::min(static_cast<double>(window_w) / cfg.logical_width,
                                 static_cast<double>(window_h) / cfg.logical_height);
    auto full_map_scale = static_cast<double>(area_w) / max_game_w;
    window_frame_scale = std::max(static_cast<int>(std::lround(normal_scale / full_map_scale)), 1);

    auto [max_w, max_h] = getFullMapSize(cfg);
    game_window_w = std::min(max_w, max_game_w);
    // Use the aspect ratio of the area available in the window, so that the game window
    // fills it
    game_window_h = std::min(max_h, static_cast<int>(static_cast<double>(area_h) *
                                                      game_window_w / area_w));
}

Rect DisplayConfig::getGameArea() const
{
    auto [window_w, window_h] = getWindowSize();
    return {reserved_side, 0, window_w - 2 * reserved_side, window_h - reserved_bottom};
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
    auto const area = getGameArea();
    auto const window_w = area.w;
    auto const window_h = area.h;
    auto scale_w = static_cast<float>(window_w) / game_window_w;
    auto scale_h = static_cast<float>(window_h) / game_window_h;
    auto scale = std::min(scale_w, scale_h);

    renderer->setScale(1, 1);
    Rect rect{area.x + (window_w - static_cast<int>(game_window_w * scale)) / 2,
             (window_h - static_cast<int>(game_window_h * scale)) / 2,
             static_cast<int>(game_window_w * scale),
             static_cast<int>(game_window_h * scale)};
    renderer->setViewport(&rect);
    renderer->setScale(scale, scale);
}

std::pair<int, int> DisplayConfig::setWindowFrameViewport() const
{
    auto const area = getGameArea();
    auto const window_w = area.w;
    auto const window_h = area.h;
    auto scale_w = static_cast<float>(window_w) / game_window_w;
    auto scale_h = static_cast<float>(window_h) / game_window_h;
    auto scale = std::min(scale_w, scale_h);

    auto scoreboard_w = Config::SCOREBOARD_WIDTH * MapPos::PIXELS_PER_CHAR * window_frame_scale;
    auto viewport_w = static_cast<int>((game_window_w - scoreboard_w) * scale);
    renderer->setScale(1, 1);
    Rect rect{area.x + (window_w - static_cast<int>(game_window_w * scale)) / 2,
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
    auto const area = getGameArea();
    auto const window_w = area.w;
    auto const window_h = area.h;
    auto scale_w = static_cast<float>(window_w) / game_window_w;
    auto scale_h = static_cast<float>(window_h) / game_window_h;
    auto scale = std::min(scale_w, scale_h);

    auto scoreboard_w = Config::SCOREBOARD_WIDTH * MapPos::PIXELS_PER_CHAR * window_frame_scale;
    auto frame_x = area.x + (window_w - static_cast<int>(game_window_w * scale)) / 2;
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
