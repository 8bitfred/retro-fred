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

    MapPos const &getFrame() const { return gFrame; }

    bool needsNewLeftCol() const { return gFrame.cx == gNewLeftCol_cx; }
    bool needsNewRightCol() const { return gFrame.cx == gNewRightCol_cx; }
    int newRightColOffset() const { return gNewRightColOffset; }
    bool needsNewTopRow() const { return gFrame.cy == gNewTopRow_cy; }
    bool needsNewBottomRow() const { return gFrame.cy == gNewBottomRow_cy; }
    int newBottomRowOffset() const { return gNewBottomRowOffset; }
    void left() { gFrame.left(); }
    void right() { gFrame.right(); }
    void up() { gFrame.up(); }
    void down() { gFrame.down(); }

private:
    // Position of F, in map coordinates
    MapPos gFrame;
    // Position of the game window
    ScreenPos top_left;      // (x2, y1), top left corner of the game window
    ScreenPos bottom_right;  // (x1, y2), bottom right corner of the game window
    // Position of the Fred character in the screen
    ScreenPos gFredPos;
    // Offset of the Fred character with respect to gFrame
    int gFredOffset_x, gFredOffset_y;
    // Position of the F point in screen coordinates
    ScreenPos gFramePos;
    // Value of gFrame.cx when a new column becomes visible when moving left
    int gNewLeftCol_cx;
    // Value of gFrame.cy when a new row becomes visible when moving up
    int gNewTopRow_cy;
    // Value of gFrame.cx when a new column becomes visible when moving right
    int gNewRightCol_cx;
    // Offset from gFrame.x of the position of the new column that becomes visible when
    // moving to the right.
    int gNewRightColOffset;
    // Value of gFrame.cy when a new row becomes visible when moving down
    int gNewBottomRow_cy;
    // Offset from gFrame.y of the position of the new row that becomes visible when
    // moving to the bottom.
    int gNewBottomRowOffset;
};
