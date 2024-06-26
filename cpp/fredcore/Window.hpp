#pragma once

#include "Coord.hpp"
#include "GameWindow.hpp"
#include "GameEvent.hpp"
#include "sdl.hpp"

struct Config;
struct SDL_Renderer;
class TextureManager;
class GameBase;
class DisplayConfig;

class Window
{
    // Position and size of the game window in the screen
    GameWindow game_window;
    // Offset, in pixels, to the center cell (position of Fred) relative to the top left
    // corner of the window.
    int center_offset_x = 0, center_offset_y = 0;
    // User offset
    int user_offset_x = 0, user_offset_y = 0;
    // Minimum and maximum values for window_pos
    MapPixelPos min_window_pos, max_window_pos;

    void drawMinimap(GameBase const &game, SDL_Renderer *renderer, int x, int y) const;

public:
    Window(Config const &cfg, SDL_Rect const &game_window_rect);

    void addUserOffset(int delta_x, int delta_y);
    void resetUserOffset();
    void update(EventMask event_mask);
    void setWindowPos(MapPos const &ref_pos);
    GameWindow const &getGameWindow() const { return game_window; }
    CellPos getCenter() const;

    void renderFrame(GameBase const &game, DisplayConfig const &display_cfg,
                     TextureManager const &tmgr) const;
};
