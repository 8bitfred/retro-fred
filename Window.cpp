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
//     0 |F      |       |       |       |       |       |       |       |       |
//     1 |  S X X|X X X X|X X X X|X X X X|X X X X|X X X X|X X X X|X X X X|X      |
//   0 2 |  X    |       |       |       |       |       |    X  |       |X      |
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
// Our main character, Fred, is always placed on the center of the screen. Local variable
// fred_pos contains the screen coordinates of Fred.
//
// We use the F point to determine which part of the map should be shown in the scren. We
// define the F point to be an integer number of cells to the left and to the top of Fred,
// such that the cell of the F point is always visible in the game window. Member variable
// map_pos stores the position of F in map coordinates (x, y, cx, cy). To move Fred around
// the map we just update the coordinates of F. Since Fred is always a given number of
// cells from F, Fred moves as F moves.
//
// The S point above is the top left corner of the screen. Member variable screen_pos
// contains the position of F with respect to S.
//
Window::Window(Config const &cfg)
{
    window_width = cfg.window_width;
    window_height = cfg.window_height;

    // Screen coordinates of the game window
    //   (in the example: top_left={8,8} bottom right={200,184})
    top_left.x = MapPixelPos::PIXELS_PER_CHAR;  // x2
    top_left.y = MapPixelPos::PIXELS_PER_CHAR;  // y1
    bottom_right.x = cfg.window_width - SCOREBOARD_WIDTH * MapPixelPos::PIXELS_PER_CHAR; // x1
    bottom_right.y = cfg.window_height - MapPixelPos::PIXELS_PER_CHAR;              // y2

    // Position of Fred: in the center of the screen, rounded down to a character
    //   (in the example: fred_pos.x = 88, fred_pos.y = 72)
    auto center_x = (top_left.x + bottom_right.x - MapPixelPos::CELL_WIDTH_PIXELS) / 2;
    center_cell.x = round_down(center_x, MapPixelPos::PIXELS_PER_CHAR);
    auto center_y = (top_left.y + bottom_right.y - MapPixelPos::CELL_HEIGHT_PIXELS) / 2;
    center_cell.y = round_down(center_y, MapPixelPos::PIXELS_PER_CHAR);
    // We want F to be an integer number of cells fom the left and top of the Fred sprite,
    // so we calculate how many cells Fred is from F
    //   (in the example fred_offset_x = 3, fred_offset_y = 2)
    fred_offset_x = ceil_of_div(center_cell.x - top_left.x, MapPixelPos::CELL_WIDTH_PIXELS);
    fred_offset_y = ceil_of_div(center_cell.y - top_left.y, MapPixelPos::CELL_HEIGHT_PIXELS);
    // Coordinates of F, with reference to the screen (S), based on the position of Fred
    // in the Screen
    //   (in the example: screen_pos.x = -8, screen_pos.y = -8)
    screen_pos.x = center_cell.x - fred_offset_x * MapPixelPos::CELL_WIDTH_PIXELS;
    screen_pos.y = center_cell.y - fred_offset_y * MapPixelPos::CELL_HEIGHT_PIXELS;
    // Note that screen_pos.x and screen_pos.y are always less than or equal to 0.
}

ScreenPos Window::getScreenPosOf(MapPos const &sprite_pos) const
{
    ScreenPos spos;
    spos.x = screen_pos.x +
             (sprite_pos.getCharX() - map_pos.getCharX()) * MapPixelPos::PIXELS_PER_CHAR;
    spos.y = screen_pos.y +
             (sprite_pos.getCharY() - map_pos.getCharY()) * MapPixelPos::PIXELS_PER_CHAR;
    return spos;
}

void Window::adjustFramePos(MapPos fred_pos)
{
    map_pos = MapPos{fred_pos.x() - fred_offset_x,
                     fred_pos.y() - fred_offset_y,
                     fred_pos.cx(),
                     fred_pos.cy()};
    map_pos.yadd(-1);
}

void Window::renderFrame(Game &game, SDL_Renderer *renderer,
                         TextureManager const &tmgr)
{
    SDL_Texture *base_window = tmgr.get(TextureID::FRAME_BASE);
    Uint32 texture_format;
    SDL_QueryTexture(base_window, &texture_format, nullptr, nullptr, nullptr);

    SDL_Rect const window_char = {0, 0, 8, 8};
    for (int x = 0; x < window_width; x += 8)
    {
        SDL_Rect dst_rect = {x, 0, 8, 8};
        auto status = SDL_RenderCopy(renderer, base_window, &window_char, &dst_rect);
        if (status < 0)
            throw sdl::Error();
        dst_rect.y = window_height - 8;
        status = SDL_RenderCopy(renderer, base_window, &window_char, &dst_rect);
        if (status < 0)
            throw sdl::Error();
    }

    for (int y = 0; y < window_height; y += 8)
    {
        SDL_Rect dst_rect = {0, y, 8, 8};
        auto status = SDL_RenderCopy(renderer, base_window, &window_char, &dst_rect);
        if (status < 0)
            throw sdl::Error();
        for (int x = window_width - 56; x < window_width; x += 8)
        {
            dst_rect.x = x;
            status = SDL_RenderCopy(renderer, base_window, &window_char, &dst_rect);
            if (status < 0)
                throw sdl::Error();
        }
    }

    SDL_Rect src_scoreboard{8, 8, 40, 176};
    SDL_Rect dst_scoreboard{window_width - 48, 8, 40, 176};
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