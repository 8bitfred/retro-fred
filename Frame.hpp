#pragma once

#include "Coord.hpp"

struct Config;

class Frame
{
public:
    static constexpr int SCOREBOARD_WIDTH = 7;
    explicit Frame(Config const &cfg);

    ScreenPos const& getTopLeft() const { return top_left; }
    ScreenPos const& getBottomRight() const { return bottom_right; }

    ScreenPos getScreenPosOf(MapPos const &sprite_pos) const;

    MapPos const &gFrame() const { return map_pos; }
    void moveFrame(int deltax, int deltay)
    {
        map_pos.xadd(deltax);
        map_pos.yadd(deltay);
    }
    void adjustFramePos(MapPos fred_pos);

    bool needsNewLeftCol() const { return map_pos.cx == new_left_col_cx; }
    bool needsNewRightCol() const { return map_pos.cx == new_right_col_cx; }
    int newRightColOffset() const { return new_right_col_offset; }
    bool needsNewTopRow() const { return map_pos.cy == new_top_row_cy; }
    bool needsNewBottomRow() const { return map_pos.cy == new_bottom_row_cy; }
    int newBottomRowOffset() const { return new_bottom_row_offset; }

    int getFredOffsetX() const { return fred_offset_x; }
    int getFredOffsetY() const { return fred_offset_y; }

private:
    // Position of F, in map coordinates
    MapPos map_pos;
    // Position of the game window
    ScreenPos top_left;      // (x2, y1), top left corner of the game window
    ScreenPos bottom_right;  // (x1, y2), bottom right corner of the game window
    // Offset of the Fred character with respect to F, in cells
    int fred_offset_x, fred_offset_y;
    // Position of the F point in screen coordinates
    ScreenPos screen_pos;
    // Value of map_pos.cx when a new column becomes visible when moving left
    int new_left_col_cx;
    // Value of map_pos.cy when a new row becomes visible when moving up
    int new_top_row_cy;
    // Value of map_pos.cx when a new column becomes visible when moving right
    int new_right_col_cx;
    // Offset from map_pos.x of the position of the new column that becomes visible when
    // moving to the right.
    int new_right_col_offset;
    // Value of map_pos.cy when a new row becomes visible when moving down
    int new_bottom_row_cy;
    // Offset from map_pos.y of the position of the new row that becomes visible when
    // moving to the bottom.
    int new_bottom_row_offset;
};
