#include "Window.hpp"
#include "Config.hpp"
#include "TextureManager.hpp"
#include "sdl.hpp"
#include "Game.hpp"
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
Window::Window(Config const &cfg)
    : total_width(cfg.window_width)
    , total_height(cfg.window_height)
    , window_rect{MapPos::PIXELS_PER_CHAR, MapPos::PIXELS_PER_CHAR,
                  total_width - (SCOREBOARD_WIDTH + 1) * MapPos::PIXELS_PER_CHAR,
                  total_height - 2 * MapPos::PIXELS_PER_CHAR}
{
    // Position of the center cell (for Fred): in the center of the screen, rounded down
    // to a character (in the example: center_offset_x = 80, center_offset_y = 64)
    center_offset_x = round_down((window_rect.w - MapPos::CELL_WIDTH_PIXELS) / 2,
                                 MapPos::PIXELS_PER_CHAR);
    center_offset_y = round_down((window_rect.h - MapPos::CELL_HEIGHT_PIXELS) / 2,
                                 MapPos::PIXELS_PER_CHAR);

    if (cfg.debug_map)
    {
        std::cout << "total_width=" << total_width
                  << " total_height=" << total_height
                  << std::endl
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

void Window::setWindowPos(MapPos const &ref_pos)
{
    window_pos.x = ref_pos.px() - center_offset_x + user_offset_x;
    window_pos.y = ref_pos.py() - center_offset_y + user_offset_y;
}

ScreenPos Window::getScreenPosOf(MapPos const &sprite_pos) const
{
    ScreenPos spos;
    spos.x = window_rect.x + sprite_pos.px() - window_pos.x;
    spos.y = window_rect.y + sprite_pos.py() - window_pos.y;
    return spos;
}

CellPos Window::getCenter() const
{
    return CellPos{(window_pos.x + center_offset_x) / MapPos::CELL_WIDTH_PIXELS,
                   (window_pos.y + center_offset_y) / MapPos::CELL_HEIGHT_PIXELS};
}

void Window::renderFrame(Game &game, SDL_Renderer *renderer,
                         TextureManager const &tmgr)
{
    SDL_Texture *base_window = tmgr.get(TextureID::FRAME_BASE);
    Uint32 texture_format;
    SDL_QueryTexture(base_window, &texture_format, nullptr, nullptr, nullptr);

    SDL_Rect const window_char = {0, 0, MapPos::PIXELS_PER_CHAR, MapPos::PIXELS_PER_CHAR};
    for (int x = 0; x < total_width; x += MapPos::PIXELS_PER_CHAR)
    {
        SDL_Rect dst_rect = {x, 0, MapPos::PIXELS_PER_CHAR, MapPos::PIXELS_PER_CHAR};
        auto status = SDL_RenderCopy(renderer, base_window, &window_char, &dst_rect);
        if (status < 0)
            throw sdl::Error();
        dst_rect.y = total_height - MapPos::PIXELS_PER_CHAR;
        status = SDL_RenderCopy(renderer, base_window, &window_char, &dst_rect);
        if (status < 0)
            throw sdl::Error();
    }

    for (int y = 0; y < total_height; y += MapPos::PIXELS_PER_CHAR)
    {
        SDL_Rect dst_rect = {0, y, MapPos::PIXELS_PER_CHAR, MapPos::PIXELS_PER_CHAR};
        auto status = SDL_RenderCopy(renderer, base_window, &window_char, &dst_rect);
        if (status < 0)
            throw sdl::Error();
        for (int x = total_width - SCOREBOARD_WIDTH * MapPos::PIXELS_PER_CHAR; 
            x < total_width; x += MapPos::PIXELS_PER_CHAR)
        {
            dst_rect.x = x;
            status = SDL_RenderCopy(renderer, base_window, &window_char, &dst_rect);
            if (status < 0)
                throw sdl::Error();
        }
    }

    SDL_Rect src_scoreboard{8, 8, 40, 176};
    SDL_Rect dst_scoreboard{total_width - (SCOREBOARD_WIDTH - 1) * MapPos::PIXELS_PER_CHAR, 
                            MapPos::PIXELS_PER_CHAR, 40, 176};
    auto status = SDL_RenderCopy(renderer, base_window, &src_scoreboard, &dst_scoreboard);
    if (status < 0)
        throw sdl::Error();

    char buf[3];
    std::snprintf(buf, sizeof(buf), "%02d", game.getBulletCount());
    tmgr.renderText(renderer, buf,
                    dst_scoreboard.x + 3 * 8, dst_scoreboard.y,
                    0, 0, 0);
    std::snprintf(buf, sizeof(buf), "%02d", game.getLevel());
    tmgr.renderText(renderer, buf,
                    dst_scoreboard.x + 3 * 8, dst_scoreboard.y + 8,
                    0, 0, 0);
    std::snprintf(buf, sizeof(buf), "%02d", game.getFredPos().y());
    tmgr.renderText(renderer, buf,
                    dst_scoreboard.x + 3 * 8, dst_scoreboard.y + 2*8,
                    0, 0, 0);
    tmgr.renderScore(renderer, game.getScore(),
                     dst_scoreboard.x + 3, dst_scoreboard.y + 17 * 8 + 2,
                     206, 206, 206);
    tmgr.renderScore(renderer, game.getHighScore(),
                     dst_scoreboard.x + 3, dst_scoreboard.y + 20 * 8 + 2,
                     206, 206, 206);

    SDL_Texture *power_texture = tmgr.get(TextureID::POWER);
    for (int i = game.getPower(); i < 15; ++i)
    {
        SDL_Rect power_rect = {dst_scoreboard.x + 8 * (i % 5),
                               dst_scoreboard.y + 8 * (12 + (i / 5)),
                               8, 8};
        if (SDL_RenderCopy(renderer, power_texture, nullptr, &power_rect) < 0)
            throw sdl::Error();
    }
    drawMinimap(game, renderer, dst_scoreboard.x, dst_scoreboard.y + 5 * 8);
}

void Window::drawMinimap(Game &game, SDL_Renderer *renderer, int x, int y)
{
    auto minimap_pos = game.getMinimapPos();
    if (!minimap_pos)
        return;
    sdl::ColorGuard color_guard(renderer, 206, 206, 206, 255);
    SDL_Rect dst{x, y, 2, 2};
    for (int i = -10; i < 10; ++i)
    {
        for (int j = -10; j < 10; ++j)
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
            dst.x += 2;
        }
        dst.x = x;
        dst.y += 2;
    }
}