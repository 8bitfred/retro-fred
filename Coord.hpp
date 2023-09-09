#pragma once

#include <cstdint>
#include <cassert>

// Coordinates of a cell within the map
struct CellPos
{
    int x = 0;
    int y = 0;
};

// Coordinates of a character withing the map
class MapPos
{
    int char_x = 0;
    int char_y = 0;
    static constexpr int div(int a, int b) { return a / b + (a < 0 ? -1 : 0); }
    static constexpr int mod(int a, int b) { return a % b + (a < 0 ? b : 0); }

public:
    static constexpr int CELL_WIDTH = 4;
    static constexpr int CELL_HEIGHT = 5;
    static constexpr int PIXELS_PER_CHAR = 8;
    constexpr MapPos() = default;
    constexpr MapPos(int x, int y, int cx, int cy)
        : char_x(x * CELL_WIDTH + cx), char_y(y * CELL_HEIGHT + cy) {}
    constexpr int x() const { return div(char_x, CELL_WIDTH); }
    constexpr int y() const { return div(char_y, CELL_HEIGHT); }
    constexpr int cx() const { return mod(char_x, CELL_WIDTH); }
    constexpr int cy() const { return mod(char_y, CELL_HEIGHT); }
    constexpr int getCharX() const { return char_x; }
    constexpr int getCharY() const { return char_y; }
    constexpr int px() const { return char_x * PIXELS_PER_CHAR; }
    constexpr int py() const { return char_y * PIXELS_PER_CHAR; }
    constexpr CellPos cellPos(int offset_x = 0, int offset_y = 0) const
    {
        return {x() + offset_x, y() + offset_y};
    }
    constexpr void xadd(int delta) { char_x += delta; }
    constexpr void yadd(int delta) { char_y += delta; }
};

// Coordinates in pixels, referenced to the map
// TODO: get rid of MapPixelPos, and use only MapPos
struct MapPixelPos
{
    static constexpr int PIXELS_PER_CHAR = MapPos::PIXELS_PER_CHAR;
    static constexpr int CELL_WIDTH_PIXELS = MapPos::CELL_WIDTH * PIXELS_PER_CHAR;
    static constexpr int CELL_HEIGHT_PIXELS = MapPos::CELL_HEIGHT * PIXELS_PER_CHAR;
    int x = 0;
    int y = 0;
    MapPixelPos() = default;
    MapPixelPos(int x, int y) : x(x), y(y) {}
    explicit MapPixelPos(MapPos map_pos)
        : x(map_pos.px())
        , y(map_pos.py()) {}
};

// Coordinates in pixels, referenced to the screen
struct ScreenPos
{
    int x = 0;
    int y = 0;
    ScreenPos() = default;
    ScreenPos(int x, int y) : x(x), y(y) {}
};


// Convenience function to get the celing of a division
inline constexpr int ceil_of_div(int x, int y) {
    assert(y > 1);
    return (x + y - 1) / y;
}

inline constexpr int round_down(int x, int y)
{
    assert(x > 1);
    return (x / y) * y;
}

inline constexpr int round_up(int x, int y)
{
    assert(x > 1);
    return ceil_of_div(x, y) * y;
}
