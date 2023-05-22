#include "game_map.hpp"
#include <iostream>
#include <string>

game_map::game_map(std::minstd_rand &random_engine, int size_x, int size_y)
    : size_x(size_x), size_y(size_y), contents(size_x * size_y)
{
    if ((size_x % 2) != 1 || size_y < 5)
        throw std::runtime_error("invalid size_x");
    if ((size_y % 2) != 1 || size_y < 5)
        throw std::runtime_error("invalid size_y");

    std::uniform_int_distribution<> distrib(static_cast<int>(cell::stone1),
                                            static_cast<int>(cell::stone3));
    for (auto &c : contents)
        c = static_cast<cell>(distrib(random_engine));

    std::vector<cell_pos> forks;
    cell_pos pos{2*((size_x - 3) / 4) + 1, 2*((size_y - 3) / 4) + 1};
    set_cell(pos, cell::empty);
    enum class dir { left, right, up, down };
    dir candidates[4];
    while (true) {
        int cand_count = 0;
        auto move_left = pos.hmove(-2);
        auto move_right = pos.hmove(2);
        auto move_up = pos.vmove(-2);
        auto move_down = pos.vmove(2);
        if (move_left.x > 0 && get_cell(move_left) != cell::empty)
            candidates[cand_count++] = dir::left;
        if (move_right.x < size_x && get_cell(move_right) != cell::empty)
            candidates[cand_count++] = dir::right;
        if (move_up.y > 0 && get_cell(move_up) != cell::empty)
            candidates[cand_count++] = dir::up;
        if (move_down.y < size_y && get_cell(move_down) != cell::empty)
            candidates[cand_count++] = dir::down;
        if (cand_count == 0) {
            if (forks.empty())
                break;
            pos = forks.back();
            forks.pop_back();
            continue;
        }
        dir move;
        if (cand_count == 1)
            move = candidates[0];
        else {
            forks.push_back(pos);
            std::uniform_int_distribution<> selector(0, cand_count - 1);
            move = candidates[selector(random_engine)];
        }
        cell_pos step1, step2;
        switch (move) {
            case dir::left:
                step1 = pos.hmove(-1);
                step2 = move_left;
                break;
            case dir::right:
                step1 = pos.hmove(1);
                step2 = move_right;
                break;
            case dir::up:
                step1 = pos.vmove(-1);
                step2 = move_up;
                break;
            case dir::down:
                step1 = pos.vmove(1);
                step2 = move_down;
                break;
        }
        set_cell(step1, cell::empty);
        set_cell(step2, cell::empty);
        pos = step2;


        for (int x = 0; x < size_x; ++x) {
            for (int y = 0; y < size_y; ++y) {
                auto c = get_cell(cell_pos{ x, y });
                if (c == cell::empty)
                    std::cout << " ";
                else
                    std::cout << "X";
            }
            std::cout << std::endl;
        }
        std::string line;
        std::getline(std::cin, line);


    }
}

game_map::cell game_map::get_cell(cell_pos const &pos)
{
    if (pos.y < 0)
        return cell::sky;
    if (pos.x < 0 || pos.x >= size_x || pos.y >= size_y)
        return cell::sand;
    return contents[pos.y * size_x + pos.x];
}

void game_map::set_cell(cell_pos const& pos, cell c)
{
    contents[pos.y * size_x + pos.x] = c;
}
