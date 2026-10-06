#pragma once

#include "Coord.hpp"
#include "Geometry.hpp"

class GameWindow
{
    // Position and size of the game window in the screen
    Rect window_rect;
    // Map coordinates, in pixels, of the top left corner of the game window
    MapPixelPos window_pos;

public:
    explicit GameWindow(Rect const &window_rect)
        : window_rect(window_rect) {}
    Rect const &getWindowRect() const { return window_rect; }
    MapPixelPos const &getPos() const { return window_pos; }
    void setPos(MapPixelPos const &pos) { window_pos = pos; }
    ScreenPos getScreenPosOf(MapPos const &sprite_pos) const
    {
        return ScreenPos{window_rect.x + sprite_pos.px() - window_pos.x,
                         window_rect.y + sprite_pos.py() - window_pos.y};
    }

    Rect getScreenRect(ScreenPos spos, Rect const &bounding_box) const
    {
        return Rect{spos.x + bounding_box.x,
                    spos.y + bounding_box.y,
                    bounding_box.w,
                    bounding_box.h};
    }
};
