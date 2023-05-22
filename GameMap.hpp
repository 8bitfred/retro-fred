#pragma once

#include <stdexcept>
#include <cstdint>
#include <vector>
#include <random>


class GameMap
{
public:
    enum class Cell : std::uint8_t
    {
        empty,
        stone1,
        stone2,
        stone3,
        rope_start,
        rope_middle,
        rope_end,
        sky,
        sand,
        trapdoor
    };

    struct CellPos {
        int x = 0, y = 0;
        CellPos hmove(int delta) const { return { x + delta, y }; }
        CellPos vmove(int delta) const { return { x, y + delta }; }
    };
    GameMap(std::minstd_rand &random_engine, int size_x, int size_y);
    Cell getCell(CellPos const& pos);
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    int width, height;
    std::vector<Cell> contents;
    void setCell(CellPos const& pos, Cell c);
};
