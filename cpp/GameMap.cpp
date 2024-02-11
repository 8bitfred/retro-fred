#include "GameMap.hpp"
#include "TextureManager.hpp"
#include "GameWindow.hpp"
#include "Config.hpp"

std::uint8_t GameMap::debug_map[] = {
    1, 2, 3, 1, 3, 3, 2, 3, 1, 2, 1, 1, 3, 1, 2, 1, 3, 1, 1, 1, 3, 1, 1, 2, 3, 9, 1, 3, 2, 2, 1, 1, 
    1, 4, 0, 0, 0, 0, 0, 4, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 4, 0, 0, 0, 4, 
    3, 5, 2, 2, 1, 3, 1, 5, 1, 5, 1, 2, 3, 3, 1, 3, 1, 3, 1, 2, 1, 3, 1, 2, 1, 3, 2, 5, 1, 1, 1, 5, 
    2, 5, 0, 0, 0, 4, 3, 6, 3, 6, 0, 4, 1, 0, 0, 4, 0, 0, 0, 0, 1, 4, 0, 4, 1, 4, 0, 6, 1, 4, 1, 5, 
    2, 5, 3, 1, 3, 5, 2, 3, 1, 2, 1, 5, 3, 1, 2, 5, 1, 2, 2, 1, 1, 5, 1, 5, 1, 5, 1, 2, 3, 5, 3, 5, 
    1, 6, 0, 0, 3, 6, 0, 0, 0, 0, 0, 6, 0, 4, 2, 6, 0, 0, 0, 4, 3, 5, 3, 6, 0, 6, 1, 0, 0, 6, 0, 6, 
    3, 2, 1, 2, 1, 3, 2, 2, 1, 1, 1, 2, 3, 5, 3, 2, 2, 1, 3, 5, 2, 5, 2, 1, 3, 3, 3, 2, 1, 2, 1, 3, 
    2, 4, 0, 0, 0, 4, 2, 4, 0, 0, 0, 4, 1, 6, 0, 0, 0, 0, 0, 6, 3, 6, 0, 4, 0, 0, 0, 0, 0, 0, 0, 4, 
    3, 5, 3, 2, 1, 5, 2, 5, 3, 1, 2, 5, 2, 3, 2, 2, 3, 2, 3, 2, 1, 3, 2, 5, 2, 3, 2, 1, 1, 1, 1, 5, 
    1, 6, 0, 4, 1, 6, 0, 6, 2, 4, 2, 6, 0, 0, 0, 0, 0, 4, 3, 4, 0, 4, 1, 6, 0, 0, 0, 0, 0, 0, 3, 5, 
    3, 2, 3, 5, 3, 2, 3, 3, 1, 5, 2, 2, 1, 3, 3, 3, 1, 5, 1, 5, 1, 5, 1, 1, 1, 3, 2, 3, 2, 3, 2, 5, 
    1, 4, 0, 6, 1, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 4, 3, 6, 0, 6, 1, 6, 0, 0, 0, 0, 0, 4, 1, 4, 0, 5, 
    3, 5, 2, 3, 3, 1, 2, 3, 1, 1, 1, 2, 3, 3, 1, 5, 1, 2, 1, 1, 2, 1, 3, 2, 3, 1, 1, 5, 2, 5, 1, 5, 
    1, 6, 0, 0, 0, 4, 1, 4, 0, 0, 0, 0, 0, 4, 0, 6, 0, 0, 0, 4, 1, 4, 1, 4, 0, 0, 0, 6, 1, 5, 2, 5, 
    1, 1, 1, 1, 2, 5, 1, 5, 1, 2, 1, 1, 1, 5, 1, 3, 1, 2, 1, 5, 3, 5, 1, 5, 2, 1, 1, 1, 2, 5, 1, 5, 
    2, 4, 1, 4, 0, 6, 1, 5, 0, 4, 1, 4, 0, 6, 1, 0, 0, 4, 2, 6, 0, 6, 1, 5, 1, 0, 0, 4, 0, 6, 2, 6, 
    1, 5, 2, 5, 1, 1, 2, 5, 3, 5, 1, 5, 3, 1, 3, 1, 1, 5, 2, 1, 3, 3, 1, 5, 1, 1, 1, 5, 3, 1, 1, 1, 
    3, 5, 1, 5, 1, 0, 0, 6, 1, 5, 3, 5, 2, 0, 0, 4, 1, 6, 0, 0, 0, 0, 0, 6, 1, 4, 0, 6, 2, 4, 0, 4, 
    3, 5, 3, 5, 1, 3, 3, 2, 1, 5, 3, 5, 1, 1, 2, 5, 3, 1, 1, 1, 3, 1, 3, 1, 2, 5, 1, 3, 1, 5, 1, 5, 
    3, 5, 2, 6, 0, 0, 0, 0, 0, 6, 1, 6, 0, 4, 2, 6, 0, 0, 0, 0, 0, 4, 0, 4, 2, 6, 0, 4, 1, 5, 1, 5, 
    3, 5, 3, 2, 1, 1, 1, 2, 2, 3, 3, 2, 2, 5, 3, 3, 1, 2, 1, 1, 2, 5, 3, 5, 2, 2, 3, 5, 2, 5, 3, 5, 
    2, 5, 2, 4, 0, 4, 0, 4, 3, 4, 0, 0, 1, 6, 0, 0, 0, 4, 3, 4, 0, 6, 1, 6, 0, 4, 2, 6, 0, 6, 1, 5, 
    1, 5, 1, 5, 3, 5, 1, 5, 3, 5, 3, 1, 1, 1, 1, 2, 3, 5, 3, 5, 2, 3, 2, 1, 1, 5, 3, 2, 1, 3, 1, 5, 
    2, 5, 0, 6, 1, 5, 3, 5, 3, 5, 2, 4, 0, 0, 0, 0, 0, 6, 1, 5, 3, 4, 1, 4, 0, 6, 3, 4, 0, 4, 1, 5, 
    2, 5, 1, 1, 3, 5, 3, 5, 1, 5, 1, 5, 1, 3, 2, 3, 1, 1, 3, 5, 3, 5, 3, 5, 1, 2, 1, 5, 2, 5, 3, 5, 
    3, 6, 0, 0, 1, 5, 2, 6, 0, 5, 2, 5, 1, 0, 0, 4, 1, 4, 0, 5, 3, 5, 2, 5, 3, 0, 0, 5, 3, 6, 0, 5, 
    1, 2, 2, 2, 2, 5, 1, 1, 2, 5, 1, 5, 1, 2, 1, 5, 1, 5, 1, 5, 1, 5, 2, 5, 1, 1, 1, 5, 2, 3, 1, 5, 
    2, 4, 0, 0, 0, 5, 1, 0, 0, 6, 2, 6, 0, 4, 2, 6, 0, 6, 3, 5, 1, 5, 2, 6, 0, 4, 3, 6, 0, 4, 3, 5, 
    1, 5, 2, 3, 3, 5, 1, 2, 2, 1, 3, 1, 3, 5, 3, 3, 2, 1, 3, 5, 2, 5, 2, 1, 1, 5, 2, 3, 1, 5, 3, 5, 
    3, 5, 2, 0, 0, 6, 2, 4, 0, 0, 0, 0, 0, 6, 2, 4, 0, 0, 0, 6, 2, 5, 0, 0, 2, 6, 0, 0, 0, 6, 3, 5, 
    1, 5, 3, 3, 1, 2, 2, 5, 3, 3, 2, 3, 1, 3, 3, 5, 2, 1, 1, 2, 1, 5, 1, 2, 1, 1, 1, 3, 2, 1, 3, 5, 
    2, 6, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 6, 1, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 
    2, 1, 1, 1, 1, 1, 3, 2, 3, 2, 3, 2, 1, 1, 2, 1, 1, 2, 3, 1, 3, 1, 2, 1, 1, 1, 2, 3, 1, 2, 2, 2, 
};

GameMap::GameMap(Config const& cfg, std::minstd_rand &random_engine)
    : width_minus_one(cfg.map_width - 1)
    , height(cfg.map_height)
    , cell_list(width_minus_one * height)
{
    if (cfg.debug_map)
    {
        width_minus_one = 32;
        height = 33;
        cell_list.resize(sizeof(debug_map));
        for (size_t i = 0; i < cell_list.size(); ++i)
        {
            cell_list[i] = static_cast<Cell>(debug_map[i]);
            if (cell_list[i] == Cell::TRAPDOOR)
                hatch_x = static_cast<int>(i);
        }
        return;
    }
    if ((width_minus_one % 2) != 0 || width_minus_one < 4)
        throw std::runtime_error("invalid width");
    if ((height % 2) != 1 || height < 5)
        throw std::runtime_error("invalid height");

    initializeMap(random_engine);
    createMaze(random_engine);
    setUpTheRopes();
    setHatchPosition(random_engine);
}

GameMap::Cell GameMap::getBlock(CellPos const &pos, int offset_x, int offset_y) const
{
    int x = pos.x + offset_x;
    int y = pos.y + offset_y;
    if (y < 0)
        return Cell::SKY;
    if (x < 0 || x > width_minus_one || y >= height)
        return Cell::SAND;
    if (x == width_minus_one)
        return Cell::STONE1;
    return cell_list[y * width_minus_one + x];
}

void GameMap::initializeMap(std::minstd_rand &random_engine)
{
    std::uniform_int_distribution<> distrib(static_cast<int>(Cell::STONE1),
                                            static_cast<int>(Cell::STONE3));
    for (auto &c : cell_list)
        c = static_cast<Cell>(distrib(random_engine));
}

void GameMap::createMaze(std::minstd_rand &random_engine)
{
    std::vector<CellPos> forks;
    CellPos pos{2 * ((getWidth() - 3) / 4) + 1, 2 * ((getHeight() - 3) / 4) + 1};
    setCell(pos, Cell::EMPTY);
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
        CellPos move_left{pos.x - 2, pos.y};
        CellPos move_right{pos.x + 2, pos.y};
        CellPos move_up{pos.x, pos.y - 2};
        CellPos move_down{pos.x, pos.y + 2};
        if (move_left.x > 0 && getBlock(move_left) != Cell::EMPTY)
            candidates[cand_count++] = dir::left;
        if (move_right.x < getWidth() && getBlock(move_right) != Cell::EMPTY)
            candidates[cand_count++] = dir::right;
        if (move_up.y > 0 && getBlock(move_up) != Cell::EMPTY)
            candidates[cand_count++] = dir::up;
        if (move_down.y < getHeight() && getBlock(move_down) != Cell::EMPTY)
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
            step1 = {pos.x - 1, pos.y};
            step2 = move_left;
            break;
        case dir::right:
            step1 = {pos.x + 1, pos.y};
            step2 = move_right;
            break;
        case dir::up:
            step1 = {pos.x, pos.y - 1};
            step2 = move_up;
            break;
        case dir::down:
            step1 = {pos.x, pos.y + 1};
            step2 = move_down;
            break;
        }
        setCell(step1, Cell::EMPTY);
        setCell(step2, Cell::EMPTY);
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
            if (getBlock(c2) == Cell::EMPTY)
            {
                setCell(c2, Cell::ROPE_MAIN);
                if (getBlock(c1) == Cell::ROPE_END)
                    setCell(c1, Cell::ROPE_MAIN);
                else
                    setCell(c1, Cell::ROPE_START);
                setCell(c3, Cell::ROPE_END);
            }
        }
    }
}

void GameMap::setHatchPosition(std::minstd_rand &random_engine)
{
    std::uniform_int_distribution<> distrib(1, getWidth() - 2);
    while (true) 
    {
        if (tryHatchPosition(distrib(random_engine)))
            break;
    }
}


bool GameMap::tryHatchPosition(int x)
{
    CellPos pos1{x, 1};
    CellPos pos2{pos1.x, 2};
    if (getBlock(pos1) == Cell::EMPTY && isStone(pos2))
    {
        CellPos hatch_pos{x, 0};
        setCell(hatch_pos, Cell::TRAPDOOR);
        setCell(pos1, Cell::ROPE_END);
        hatch_x = x;
        return true;
    }
    return false;
}

void GameMap::setCell(CellPos const &pos, Cell c)
{
    cell_list[pos.y * width_minus_one + pos.x] = c;
}

void GameMap::renderCell(SDL_Renderer *renderer, TextureManager const &tmgr,
                         int x, int y, Cell cell)
{
    static SDL_Rect cell_rects[] = {
        {      1,      1, 32, 40},
        {   42+1,      1, 32, 40},
        { 2*42+1,      1, 32, 40},
        {      1,   50+1, 32, 40},
        {   42+1,   50+1, 32, 40},
        { 2*42+1,   50+1, 32, 40},
        {      1, 2*50+1, 32, 40},
        {   42+1, 2*50+1, 32, 40},
        { 2*42+1, 2*50+1, 32, 40},
    };
    static_assert(std::size(cell_rects) == static_cast<int>(GameMap::Cell::TRAPDOOR));
    SDL_Rect dst = {x, y, 32, 40};
    if (cell == Cell::EMPTY)
    {
        sdl::ColorGuard guard(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &dst);
        return;
    }
    assert(static_cast<unsigned>(cell) <= static_cast<int>(GameMap::Cell::TRAPDOOR));
    if (SDL_RenderCopy(renderer, tmgr.get(TextureID::BLOCK),
                       &cell_rects[static_cast<unsigned>(cell) - 1], &dst) < 0)
        throw sdl::Error();
}


void GameMap::render(SDL_Renderer *renderer, TextureManager const &tmgr,
                     GameWindow const &game_window) const
{
    auto x = game_window.getPos().x;
    auto y = game_window.getPos().y;
    CellPos corner_cell = {x / MapPixelPos::CELL_WIDTH_PIXELS,
                           y / MapPixelPos::CELL_HEIGHT_PIXELS};
    if (x < 0)
        --corner_cell.x;
    if (y < 0)
        --corner_cell.y;

    // Round x and y down to the closest corner of a cell, then compute those coordinates
    // with respect to the corner of the screen
    int round_x = corner_cell.x * MapPixelPos::CELL_WIDTH_PIXELS - x;
    int round_y = corner_cell.y * MapPixelPos::CELL_HEIGHT_PIXELS - y;

    CellPos cell_pos = corner_cell;
    for (int cell_y = game_window.getWindowRect().y + round_y;
         cell_y < (game_window.getWindowRect().y + game_window.getWindowRect().h);
         cell_y += MapPixelPos::CELL_HEIGHT_PIXELS) {
        cell_pos.x = corner_cell.x;
        for (int cell_x = game_window.getWindowRect().x + round_x;
             cell_x < (game_window.getWindowRect().x + game_window.getWindowRect().w);
             cell_x += MapPixelPos::CELL_WIDTH_PIXELS)
        {
            renderCell(renderer, tmgr, cell_x, cell_y, getBlock(cell_pos));
            ++cell_pos.x;
        }
        ++cell_pos.y;
    }
}

bool GameMap::dbgMoveHatch(int deltax)
{
    auto hatch_pos = dbgGetHatchPos();
    for (int x = hatch_x + deltax; x > 1 && x < getWidth(); x += deltax)
    {
        if (tryHatchPosition(x))
        {
            setCell(hatch_pos, Cell::STONE1);
            setCell(CellPos{hatch_pos.x, hatch_pos.y + 1}, Cell::EMPTY);
            return true;
        }
    }
    return false;
}
