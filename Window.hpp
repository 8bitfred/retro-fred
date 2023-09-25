#pragma once

#include "Coord.hpp"

struct Config;
struct SDL_Renderer;
class TextureManager;
class Game;

class Window
{
public:
    static constexpr int SCOREBOARD_WIDTH = 7;
    explicit Window(Config const &cfg);

    ScreenPos const& getTopLeft() const { return top_left; }
    ScreenPos const& getBottomRight() const { return bottom_right; }
    ScreenPos const &getCenterCell() const { return center_cell; }
    ScreenPos getScreenPosOf(MapPos const &sprite_pos) const;

    MapPos const &gFrame() const { return map_pos; }
    void moveFrame(int deltax, int deltay)
    {
        map_pos.xadd(deltax);
        map_pos.yadd(deltay);
    }
    void adjustFramePos(MapPos fred_pos);

    int getFredOffsetX() const { return fred_offset_x; }
    int getFredOffsetY() const { return fred_offset_y; }

    // TODO: Game should be const
    void renderFrame(Game& game, SDL_Renderer *renderer,
                     TextureManager const &tmgr);

private:
    void drawMinimap(Game &game, SDL_Renderer *renderer, int x, int y);

    int window_width, window_height;

    // Position of F, in map coordinates
    MapPos map_pos;
    // Position of the game window
    ScreenPos top_left;      // (x2, y1), top left corner of the game window
    ScreenPos bottom_right;  // (x1, y2), bottom right corner of the game window
    ScreenPos center_cell;
    // Offset of the Fred character with respect to F, in cells
    int fred_offset_x, fred_offset_y;
    // Position of the F point in screen coordinates
    ScreenPos screen_pos;
};
