#include "GameMap.hpp"
#include "TextureManager.hpp"

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
        CellPos pos2{pos1.x - 1, 2};
        CellPos pos3{pos1.x, 2};
        CellPos pos4{pos1.x + 1, 2};
        if (isEmpty(pos1) && isStone(pos2) && isStone(pos3) && isStone(pos4))
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
    int x_start = x / cell_width;
    int x_offset = x % cell_width;
    if (x_offset < 0)
    {
        x_start -= 1;
        x_offset += cell_width;
    }
    int y_start = y / cell_height;
    int y_offset = y % cell_height;
    if (y_offset < 0)
    {
        y_start -= 1;
        y_offset += cell_height;
    }

    CellPos cell_pos;
    cell_pos.y = y_start;
    SDL_Rect cell_dst;
    SDL_Rect cell_src;
    cell_src.y = y_offset;
    cell_src.h = cell_height - y_offset;
    for (int rel_y = 0; rel_y < dest->h;)
    {
        cell_pos.x = x_start;
        cell_dst.y = dest->y + rel_y;
        cell_dst.h = std::min(cell_src.h, dest->h - rel_y);
        cell_src.h = cell_dst.h;

        cell_src.x = x_offset;
        cell_src.w = cell_width - x_offset;
        for (int rel_x = 0; rel_x < dest->w;)
        {
            cell_dst.x = dest->x + rel_x;
            cell_dst.w = std::min(cell_src.w, dest->w - rel_x);
            cell_src.w = cell_src.w;

            auto texture = getTextureOf(tmgr, cell_pos);
            if (texture) {
                SDL_RenderCopy(renderer, texture,
                               &cell_src, &cell_dst);
            }

            rel_x += cell_src.w;
            cell_src.x = 0;
            cell_src.w = cell_width;
            ++cell_pos.x;
        }
        ++cell_pos.y;
        rel_y += cell_src.h;
        cell_src.y = 0;
        cell_src.h = cell_height;
    }
}
