#pragma once

#include <stdexcept>
#include <cstdint>
#include <vector>
#include <random>


class game_map
{
public:
    enum class cell : std::uint8_t
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

    struct cell_pos {
        int x = 0, y = 0;
        cell_pos hmove(int delta) const { return { x + delta, y }; }
        cell_pos vmove(int delta) const { return { x, y + delta }; }
    };
    game_map(std::minstd_rand &random_engine, int size_x, int size_y);
    cell get_cell(cell_pos const& pos);

private:
    int size_x, size_y;
    std::vector<cell> contents;
    void set_cell(cell_pos const& pos, cell c);
};
