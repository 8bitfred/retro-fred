#include "Window.hpp"
#include "Config.hpp"
#include "TextureManager.hpp"
#include "sdl.hpp"
#include "Game.hpp"
#include "DisplayConfig.hpp"
#include <iostream>

// Game screen
//
// The game screen consists of a 1 characer window on all sides of the screen, a game
// window where the map and characters are shown, and a scoreboard where the score and
// other game information is displayed. The scoreboard is 7 characters wide and goes from
// the top to the bottom of the screen. We allow the size of the screen to be
// configurable, so the size of the game window varies with the screen size.
//
// In the original ZX Spectrum game the screen is 256x192 pixels (32x24 characters) and
// the game window is 192x176 pixels (24x22 characters):
//
//       |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |
//       |0|1|2|3|0|1|2|3|0|1|2|3|0|1|2|3|0|1|2|3|0|1|2|3|0|1|2|3|0|1|2|3|0|1|2|3|
//     --+-------+-------+-------+-------+-------+-------+-------+-------+-------+--
//     0 |       |       |       |       |       |       |       |       |       |
//     1 |  X X X|X X X X|X X X X|X X X X|X X X X|X X X X|X X X X|X X X X|X      |
//   0 2 |  X S  |       |       |       |       |       |    X  |       |X      |
//     3 |  X    |       |       |       |       |       |    X  |       |X      |
//     4 |  X    |       |       |       |       |       |    X  |       |X      |
//     --+-------+-------+-------+-------+-------+-------+-------+-------+-------+--
//     0 |  X    |       |       |       |       |       |    X X|X X X X|X      |
//     1 |  X    |       |       |       |       |       |    X  |M A P  |X      |
//   1 2 |  X    |       |       |       |       |       |    X  |       |X      |
//     3 |  X    |       |       |       |       |       |    X  |       |X      |
//     4 |  X    |       |       |       |       |       |    X  |       |X      |
//     --+-------+-------+-------+-------+-------+-------+-------+-------+-------+--
//     0 |  X    |       |       |       |       |       |    X  |       |X      |
//     1 |  X    |       |       |  OO   |       |       |    X  |       |X      |
//   2 2 |  X    |       |       |+ mmm  |       |       |    X X|X X X X|X      |
//     3 |  X    |       |       | ===== |       |       |    X P|O W E R|X      |
//     4 |  X    |       |       |  d d  |       |       |    X  |       |X      |
//     --+-------+-------+-------+-------+-------+-------+-------+-------+-------+--
//     0 |  X    |       |       |       |       |       |    X  |       |X      |
//     1 |  X    |       |       |       |       |       |    X  |       |X      |
//   3 2 |  X    |       |       |       |       |       |    X X|X X X X|X      |
//     3 |  X    |       |       |       |       |       |    X S|C O R E|X      |
//     4 |  X    |       |       |       |       |       |    X  |       |X      |
//     --+-------+-------+-------+-------+-------+-------+-------+-------+-------+--
//     0 |  X    |       |       |       |       |       |    X X|X X X X|X      |
//     1 |  X    |       |       |       |       |       |    X H|I G H  |X      |
//   4 2 |  X    |       |       |       |       |       |    X  |       |X      |
//     3 |  X    |       |       |       |       |       |    X X|X X X X|X      |
//     4 |  X X X|X X X X|X X X X|X X X X|X X X X|X X X X|X X X X|X X X X|X      |
//     --+-------+-------+-------+-------+-------+-------+-------+-------+-------+--
//
// Our main character, Fred, is always placed on the center of the screen.
Window::Window(Config const &cfg, SDL_Rect const &game_window_rect)
    : game_window(game_window_rect)
{
    auto const &window_rect = game_window.getWindowRect();
    // Position of the center cell (for Fred): in the center of the screen, rounded down
    // to a character (in the example: center_offset_x = 80, center_offset_y = 64)
    center_offset_x = round_down((window_rect.w - MapPos::CELL_WIDTH_PIXELS) / 2,
                                 MapPos::PIXELS_PER_CHAR);
    center_offset_y = round_down((window_rect.h - MapPos::CELL_HEIGHT_PIXELS) / 2,
                                 MapPos::PIXELS_PER_CHAR);

    if (cfg.max_resolution)
    {
        min_window_pos = {-MapPos::CELL_WIDTH_PIXELS, -MapPos::CELL_HEIGHT_PIXELS};
        max_window_pos = {
            (cfg.map_width + 1) * MapPos::CELL_WIDTH_PIXELS - window_rect.w,
            (cfg.map_height + 1) * MapPos::CELL_HEIGHT_PIXELS - window_rect.h,
        };
    }
    else
    {
        min_window_pos = { MapPos::CELL_WIDTH_PIXELS - center_offset_x, -center_offset_y };
        max_window_pos = {
            (cfg.map_width - 2) * MapPos::CELL_WIDTH_PIXELS - center_offset_x,
            (cfg.map_height - 2) * MapPos::CELL_HEIGHT_PIXELS - center_offset_y
        };
    }

    if (cfg.debug_map)
    {
        std::cout << std::endl
                  << "window_rect=(" << window_rect.x
                  << ", " << window_rect.y
                  << ", " << window_rect.w
                  << ", " << window_rect.h << ")"
                  << std::endl
                  << "center_offset_x=" << center_offset_x
                  << " center_offset_y=" << center_offset_y
                  << std::endl;
    }
}

void Window::addUserOffset(int delta_x, int delta_y)
{
    user_offset_x += delta_x;
    user_offset_y += delta_y;
}

void Window::resetUserOffset()
{
    user_offset_x = user_offset_y = 0;
}

void Window::update(EventMask event_mask)
{
    if (event_mask.check(GameEvent::DBG_LEFT))
        addUserOffset(-MapPos::CELL_WIDTH_PIXELS, 0);
    else if (event_mask.check(GameEvent::DBG_RIGHT))
        addUserOffset(MapPos::CELL_WIDTH_PIXELS, 0);
    else if (event_mask.check(GameEvent::DBG_UP))
        addUserOffset(0, -MapPos::CELL_HEIGHT_PIXELS);
    else if (event_mask.check(GameEvent::DBG_DOWN))
        addUserOffset(0, MapPos::CELL_HEIGHT_PIXELS);
    else if (event_mask.check(GameEvent::DBG_CENTER_WINDOW))
        resetUserOffset();
}

void Window::setWindowPos(MapPos const &ref_pos)
{
    auto raw_x = ref_pos.px() - center_offset_x + user_offset_x;
    auto raw_y = ref_pos.py() - center_offset_y + user_offset_y;
    game_window.setPos(MapPixelPos{std::min(std::max(raw_x, min_window_pos.x), max_window_pos.x),
                                   std::min(std::max(raw_y, min_window_pos.y), max_window_pos.y)});
}

CellPos Window::getCenter() const
{
    return CellPos{(game_window.getPos().x + center_offset_x) / MapPos::CELL_WIDTH_PIXELS,
                   (game_window.getPos().y + center_offset_y) / MapPos::CELL_HEIGHT_PIXELS};
}

void Window::renderFrame(GameBase const &game, DisplayConfig const &display_cfg,
                         TextureManager const &tmgr) const
{
    SDL_Texture *base_window = tmgr.get(TextureID::FRAME_BASE);
    Uint32 texture_format;
    SDL_QueryTexture(base_window, &texture_format, nullptr, nullptr, nullptr);

    auto [frame_w, frame_h] = display_cfg.setWindowFrameViewport();
    SDL_Rect const window_char = {0, 0, MapPos::PIXELS_PER_CHAR, MapPos::PIXELS_PER_CHAR};
    for (int x = 0; x < frame_w; x += MapPos::PIXELS_PER_CHAR)
    {
        SDL_Rect dst_rect = {x, 0, MapPos::PIXELS_PER_CHAR, MapPos::PIXELS_PER_CHAR};
        auto status = SDL_RenderCopy(display_cfg.getRenderer(), base_window, &window_char, &dst_rect);
        if (status < 0)
            throw sdl::Error();
        dst_rect.y = frame_h - MapPos::PIXELS_PER_CHAR;
        status = SDL_RenderCopy(display_cfg.getRenderer(), base_window, &window_char, &dst_rect);
        if (status < 0)
            throw sdl::Error();
    }

    for (int y = 0; y < frame_h; y += MapPos::PIXELS_PER_CHAR)
    {
        SDL_Rect dst_rect = {0, y, MapPos::PIXELS_PER_CHAR, MapPos::PIXELS_PER_CHAR};
        auto status = SDL_RenderCopy(display_cfg.getRenderer(), base_window, &window_char, &dst_rect);
        if (status < 0)
            throw sdl::Error();
    }

    auto [sb_w, sb_h] = display_cfg.setScoreboardViewport();
    //SDL_Log("frame_w=%d frame_h=%d sb_w=%d wb_h=%d", frame_w, frame_h, sb_w, sb_h);
    for (int y = 0; y < sb_h; y += MapPos::PIXELS_PER_CHAR)
    {
        SDL_Rect dst_rect = {0, y, MapPos::PIXELS_PER_CHAR, MapPos::PIXELS_PER_CHAR};
        for (int x = 0; x < sb_w; x += MapPos::PIXELS_PER_CHAR)
        {
            dst_rect.x = x;
            auto status = SDL_RenderCopy(display_cfg.getRenderer(), base_window, &window_char, &dst_rect);
            if (status < 0)
                throw sdl::Error();
        }
    }
    SDL_Rect src_scoreboard{8, 8, 40, 176};
    SDL_Rect dst_scoreboard{MapPos::PIXELS_PER_CHAR,
                            MapPos::PIXELS_PER_CHAR, 40, 176};
    auto status = SDL_RenderCopy(display_cfg.getRenderer(), base_window, &src_scoreboard, &dst_scoreboard);
    if (status < 0)
        throw sdl::Error();

    char buf[3];
    std::snprintf(buf, sizeof(buf), "%02d", game.getBulletCount());
    tmgr.renderText(display_cfg.getRenderer(), buf,
                    dst_scoreboard.x + 3 * 8, dst_scoreboard.y,
                    0, 0, 0);
    std::snprintf(buf, sizeof(buf), "%02d", game.getLevel());
    tmgr.renderText(display_cfg.getRenderer(), buf,
                    dst_scoreboard.x + 3 * 8, dst_scoreboard.y + 8,
                    0, 0, 0);
    std::snprintf(buf, sizeof(buf), "%02d", game.getFredPos().y());
    tmgr.renderText(display_cfg.getRenderer(), buf,
                    dst_scoreboard.x + 3 * 8, dst_scoreboard.y + 2*8,
                    0, 0, 0);
    tmgr.renderScore(display_cfg.getRenderer(), game.getScore(),
                     dst_scoreboard.x + 3, dst_scoreboard.y + 17 * 8 + 2,
                     206, 206, 206);
    tmgr.renderScore(display_cfg.getRenderer(), game.getHighScore(),
                     dst_scoreboard.x + 3, dst_scoreboard.y + 20 * 8 + 2,
                     206, 206, 206);

    SDL_Texture *power_texture = tmgr.get(TextureID::POWER);
    for (int i = game.getPower(); i < 15; ++i)
    {
        SDL_Rect power_rect = {dst_scoreboard.x + 8 * (i % 5),
                               dst_scoreboard.y + 8 * (12 + (i / 5)),
                               8, 8};
        if (SDL_RenderCopy(display_cfg.getRenderer(), power_texture, nullptr, &power_rect) < 0)
            throw sdl::Error();
    }
    drawMinimap(game, display_cfg.getRenderer(), dst_scoreboard.x, dst_scoreboard.y + 5 * 8);
}

void Window::drawMinimap(GameBase const &game, SDL_Renderer *renderer, int x, int y) const
{
    static constexpr int MINIMAP_MAX = 10;
    static constexpr int MINIMAP_CELL_SIZE = 2;
    auto minimap_pos = game.getMinimapPos();
    if (!minimap_pos)
        return;
    sdl::ColorGuard color_guard(renderer, 206, 206, 206, 255);
    SDL_Rect dst{x, y, MINIMAP_CELL_SIZE, MINIMAP_CELL_SIZE};
    for (int i = -MINIMAP_MAX; i < MINIMAP_MAX; ++i)
    {
        for (int j = -MINIMAP_MAX; j < MINIMAP_MAX; ++j)
        {
            if (auto cell = game.getGameMap().getBlock(*minimap_pos, j, i);
                cell == GameMap::Cell::EMPTY ||
                cell == GameMap::Cell::ROPE_START ||
                cell == GameMap::Cell::ROPE_MAIN ||
                cell == GameMap::Cell::ROPE_END ||
                (cell == GameMap::Cell::TRAPDOOR && game.getConfig().minimap_exit))
            {
                if (SDL_RenderFillRect(renderer, &dst) < 0)
                    throw sdl::Error();
            }
            dst.x += MINIMAP_CELL_SIZE;
        }
        dst.x = x;
        dst.y += MINIMAP_CELL_SIZE;
    }
    if (game.getConfig().minimap_tracker)
    {
        // coordingates of fred in a map assuming cells have size MINIMAP_CELL_SIZE pixels
        int fred_x = game.getFredPos().getCharX() * MINIMAP_CELL_SIZE / MapPos::CELL_WIDTH;
        int fred_y = game.getFredPos().getCharY() * MINIMAP_CELL_SIZE / MapPos::CELL_HEIGHT;

        int top_left_x = (minimap_pos->x - MINIMAP_MAX) * MINIMAP_CELL_SIZE;
        int top_left_y = (minimap_pos->y - MINIMAP_MAX) * MINIMAP_CELL_SIZE;

        SDL_Rect minimap_rect = {x, y, 
                                 2 * MINIMAP_MAX * MINIMAP_CELL_SIZE,
                                 2 * MINIMAP_MAX * MINIMAP_CELL_SIZE};
        SDL_Rect tracker_rect = {x + fred_x - top_left_x, y + fred_y - top_left_y,
                                 MINIMAP_CELL_SIZE, MINIMAP_CELL_SIZE};
        SDL_Rect visible_rect;
        if (SDL_IntersectRect(&minimap_rect, &tracker_rect, &visible_rect))
        {
            sdl::ColorGuard color_guard_tracker(renderer, 206, 0, 0, 255);
            if (SDL_RenderFillRect(renderer, &visible_rect) < 0)
                throw sdl::Error();
        }
    }
}
