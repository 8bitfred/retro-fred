#pragma once

#include "Coord.hpp"
#include "GameWindow.hpp"
#include "sdl.hpp"

struct Config;
struct SDL_Renderer;
class TextureManager;
class Game;

class Window
{
    // Total width of the system window (game window and window frame)
    int total_width, total_height;
    // Position and size of the game window in the screen
    GameWindow game_window;
    // Offset, in pixels, to the center cell (position of Fred) relative to the top left
    // corner of the window.
    int center_offset_x = 0, center_offset_y = 0;
    // User offset
    int user_offset_x = 0, user_offset_y = 0;
    // Minimum and maximum values for window_pos
    MapPixelPos min_window_pos, max_window_pos;

    void drawMinimap(Game const &game, SDL_Renderer *renderer, int x, int y) const;

public:
    static constexpr int SCOREBOARD_WIDTH = 7;
    Window(Config const &cfg, int total_width, int total_height);

    void addUserOffset(int delta_x, int delta_y);
    void resetUserOffset();
    void setWindowPos(MapPos const &ref_pos);
    GameWindow const &getGameWindow() const { return game_window; }
    CellPos getCenter() const;

    void renderFrame(Game const &game, SDL_Renderer *renderer,
                     TextureManager const &tmgr) const;
};
