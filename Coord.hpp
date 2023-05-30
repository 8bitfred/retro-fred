#pragma once

#include <cstdint>

// Coordinates of a cell within the map
struct CellPos
{
    int x = 0;
    int y = 0;
    CellPos hmove(int delta) const { return {x + delta, y}; }
    CellPos vmove(int delta) const { return {x, y + delta}; }
};

// Coordinates of a character withing the map
struct MapPos
{
    static constexpr int CELL_WIDTH = 4;
    static constexpr int CELL_HEIGHT = 5;
    int x = 0;
    int y = 0;
    int cx = 0;
    int cy = 0;
};

// Coordinates in pixels, referenced to the map
struct MapPixelPos
{
    static constexpr int PIXELS_PER_CHAR = 8;
    static constexpr int CELL_WIDTH_PIXELS = MapPos::CELL_WIDTH * PIXELS_PER_CHAR;
    static constexpr int CELL_HEIGHT_PIXELS = MapPos::CELL_HEIGHT * PIXELS_PER_CHAR;
    int x = 0;
    int y = 0;
    MapPixelPos() = default;
    MapPixelPos(int x, int y) : x(x), y(y) {}
    explicit MapPixelPos(MapPos map_pos)
        : x((map_pos.x * MapPos::CELL_WIDTH + map_pos.cx) * PIXELS_PER_CHAR)
        , y((map_pos.y * MapPos::CELL_HEIGHT + map_pos.cy) * PIXELS_PER_CHAR) {}
};

// Coordinates in pixels, referenced to the screen
struct ScreenPos
{
    int x = 0;
    int y = 0;
    ScreenPos() = default;
    ScreenPos(int x, int y) : x(x), y(y) {}
};
