#include "GameMap.hpp"
#include "TextureManager.hpp"
#include <iostream>

GameMap::GameMap(std::minstd_rand &random_engine, int width, int height)
    : width(width), height(height), contents(width * height)
{
    if ((width % 2) != 1 || height < 5)
        throw std::runtime_error("invalid width");
    if ((height % 2) != 1 || height < 5)
        throw std::runtime_error("invalid height");

    initializeMap(random_engine);
    createMaze(random_engine);
    setUpTheRopes();
    setHatchPosition(random_engine);
}

GameMap::Cell GameMap::getCell(CellPos const &pos) const
{
    if (pos.y < 0)
        return Cell::sky;
    if (pos.x < 0 || pos.x >= width || pos.y >= height)
        return Cell::sand;
    return contents[pos.y * width + pos.x];
}

void GameMap::initializeMap(std::minstd_rand &random_engine)
{
    std::uniform_int_distribution<> distrib(static_cast<int>(Cell::stone1),
                                            static_cast<int>(Cell::stone3));
    for (auto &c : contents)
        c = static_cast<Cell>(distrib(random_engine));
    // To preserve the look of the original game we fill the last column with type 1
    // stones.
    for (CellPos pos = {width - 1, 0}; pos.y < height; ++pos.y)
        setCell(pos, Cell::stone1);
}

void GameMap::createMaze(std::minstd_rand &random_engine)
{
    std::vector<CellPos> forks;
    CellPos pos{2 * ((width - 3) / 4) + 1, 2 * ((height - 3) / 4) + 1};
    setCell(pos, Cell::empty);
    enum class dir
    {
        left,
        right,
        up,
        down
    };
    dir candidates[4];
    while (true)
    {
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
        if (cand_count == 0)
        {
            if (forks.empty())
                break;
            pos = forks.back();
            forks.pop_back();
            continue;
        }
        dir move;
        if (cand_count == 1)
            move = candidates[0];
        else
        {
            forks.push_back(pos);
            std::uniform_int_distribution<> selector(0, cand_count - 1);
            move = candidates[selector(random_engine)];
        }
        CellPos step1, step2;
        switch (move)
        {
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

void GameMap::setUpTheRopes()
{
    CellPos c1;
    for (c1.x = 1; c1.x < getWidth(); c1.x += 2) 
    {
        for (c1.y = 1; c1.y < (getHeight() - 2); c1.y += 2)
        {
            CellPos c2{c1.x, c1.y + 1};
            CellPos c3{c1.x, c1.y + 2};
            if (getCell(c2) == Cell::empty)
            {
                setCell(c2, Cell::rope_middle);
                if (getCell(c1) == Cell::rope_end)
                    setCell(c1, Cell::rope_middle);
                else
                    setCell(c1, Cell::rope_start);
                setCell(c3, Cell::rope_end);
            }
        }
    }
}

void GameMap::setHatchPosition(std::minstd_rand &random_engine)
{
    std::uniform_int_distribution<> distrib(1, getWidth() - 2);
    while (true) 
    {
        int x = distrib(random_engine);
        CellPos pos1{x, 1};
        CellPos pos2{pos1.x, 2};
        if (isEmpty(pos1) && isStone(pos2))
        {
            CellPos hatch_pos{x, 0};
            setCell(hatch_pos, Cell::trapdoor);
            setCell(pos1, Cell::rope_end);
            return;
        }
    }
}


void GameMap::setCell(CellPos const &pos, Cell c)
{
    contents[pos.y * width + pos.x] = c;
}

SDL_Texture *GameMap::getTextureOf(TextureManager const &tmgr, CellPos const &pos) const
{
    switch (getCell(pos))
    {
    case Cell::empty:
        return nullptr;
    case Cell::stone1:
        return tmgr.get(TextureManager::TextureID::stone1);
    case Cell::stone2:
        return tmgr.get(TextureManager::TextureID::stone2);
    case Cell::stone3:
        return tmgr.get(TextureManager::TextureID::stone3);
    case Cell::rope_start:
        return tmgr.get(TextureManager::TextureID::rope_start);
    case Cell::rope_middle:
        return tmgr.get(TextureManager::TextureID::rope_middle);
    case Cell::rope_end:
        return tmgr.get(TextureManager::TextureID::rope_end);
    case Cell::sky:
        return tmgr.get(TextureManager::TextureID::sky);
    case Cell::sand:
        return tmgr.get(TextureManager::TextureID::sand);
    case Cell::trapdoor:
        return tmgr.get(TextureManager::TextureID::trapdoor);
    default:
        return nullptr;
    }
}

void GameMap::render(int x, int y, TextureManager const &tmgr,
                     SDL_Renderer *renderer, SDL_Rect const *dest)
{
    CellPos corner_cell = {x / cell_width, y / cell_height};
    if (x < 0)
        --corner_cell.x;
    if (y < 0)
        --corner_cell.y;

    // Round x and y down to the closest corner of a cell, then compute those coordinates
    // with respect to the corner of the screen
    int round_x = corner_cell.x * cell_width - x;
    int round_y = corner_cell.y * cell_height - y;

    static bool first = true;
    if (first) {
        first = false;
        std::cout << "round_x=" << round_x << " round_y=" << round_y
                  << " corner_x=" << corner_cell.x << " corner_y=" << corner_cell.y << std::endl;
    }

    CellPos cell_pos = corner_cell;
    SDL_Rect cell_rect;
    cell_rect.w = cell_width;
    cell_rect.h = cell_height;

    for (cell_rect.y = round_y; cell_rect.y < dest->h; cell_rect.y += cell_rect.h) {
        cell_pos.x = corner_cell.x;
        for (cell_rect.x = round_x; cell_rect.x < dest->w; cell_rect.x += cell_rect.w)
        {
            SDL_Rect cell_dst;
            cell_dst.x = dest->x + cell_rect.x;
            cell_dst.y = dest->y + cell_rect.y;
            cell_dst.h = cell_rect.h;
            cell_dst.w = cell_rect.w;
            auto texture = getTextureOf(tmgr, cell_pos);
            if (texture) {
                SDL_RenderCopy(renderer, texture,
                               nullptr, &cell_dst);
            }
            ++cell_pos.x;
        }
        ++cell_pos.y;
    }
}
