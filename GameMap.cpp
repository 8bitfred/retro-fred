#include "GameMap.hpp"

GameMap::GameMap(std::minstd_rand &random_engine, int width, int height)
    : width(width), height(height), contents(width * height)
{
    if ((width % 2) != 1 || height < 5)
        throw std::runtime_error("invalid width");
    if ((height % 2) != 1 || height < 5)
        throw std::runtime_error("invalid height");

    std::uniform_int_distribution<> distrib(static_cast<int>(Cell::stone1),
                                            static_cast<int>(Cell::stone3));
    for (auto &c : contents)
        c = static_cast<Cell>(distrib(random_engine));

    std::vector<CellPos> forks;
    CellPos pos{2*((width - 3) / 4) + 1, 2*((height - 3) / 4) + 1};
    setCell(pos, Cell::empty);
    enum class dir { left, right, up, down };
    dir candidates[4];
    while (true) {
        int cand_count = 0;
        auto move_left = pos.hmove(-2);
        auto move_right = pos.hmove(2);
        auto move_up = pos.vmove(-2);
        auto move_down = pos.vmove(2);
        if (move_left.x > 0 && getCell(move_left) != Cell::empty)
            candidates[cand_count++] = dir::left;
        if (move_right.x < width && getCell(move_right) != Cell::empty)
            candidates[cand_count++] = dir::right;
        if (move_up.y > 0 && getCell(move_up) != Cell::empty)
            candidates[cand_count++] = dir::up;
        if (move_down.y < height && getCell(move_down) != Cell::empty)
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
        CellPos step1, step2;
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
        setCell(step1, Cell::empty);
        setCell(step2, Cell::empty);
        pos = step2;
    }
}

GameMap::Cell GameMap::getCell(CellPos const &pos)
{
    if (pos.y < 0)
        return Cell::sky;
    if (pos.x < 0 || pos.x >= width || pos.y >= height)
        return Cell::sand;
    return contents[pos.y * width + pos.x];
}

void GameMap::setCell(CellPos const& pos, Cell c)
{
    contents[pos.y * width + pos.x] = c;
}
