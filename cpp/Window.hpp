#pragma once

#include "Coord.hpp"
#include "sdl.hpp"

struct Config;
struct DisplayConfig;
struct SDL_Renderer;
class TextureManager;
class Game;

class Window
{
    // Total width of the system window (game window and window frame)
    int total_width, total_height;
    // Position and size of the game window in the screen
    SDL_Rect window_rect;
    // Offset, in pixels, to the center cell (position of Fred) relative to the top left
    // corner of the window.
    int center_offset_x = 0, center_offset_y = 0;
    // User offset
    int user_offset_x = 0, user_offset_y = 0;
    // Position of the top left corner of the screen in the map
    MapPixelPos window_pos;
    // Minimum and maximum values for window_pos
    MapPixelPos min_window_pos, max_window_pos;

    void drawMinimap(Game const &game, SDL_Renderer *renderer, int x, int y);

public:
    static constexpr int SCOREBOARD_WIDTH = 7;
    Window(Config const &cfg, DisplayConfig const &display_cfg);

    void addUserOffset(int delta_x, int delta_y);
    void resetUserOffset();
    void setWindowPos(MapPos const &ref_pos);
    SDL_Rect const &rect() const { return window_rect; }
    MapPixelPos const &getWindowPos() const { return window_pos; }
    ScreenPos getScreenPosOf(MapPos const &sprite_pos) const;
    CellPos getCenter() const;

    void renderFrame(Game const &game, SDL_Renderer *renderer,
                     TextureManager const &tmgr);
};
