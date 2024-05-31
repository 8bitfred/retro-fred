#include "MultiDirEnemy.hpp"
#include "fredcore/GameMap.hpp"

std::pair<int, int> MultiDirEnemy::getDirDelta() const
{
    static std::pair<int, int> dir_table[] =
        {
            {-1, 0},
            {0, 1},
            {1, 0},
            {0, -1},
        };
    return dir_table[direction];
}

bool MultiDirEnemy::getRandomSense()
{
    std::uniform_int_distribution<> distrib(0, 1);
    return static_cast<bool>(distrib(random_engine));
}

void MultiDirEnemy::selectDirection(GameMap const &game_map, bool clockwise)
{
    int type = 2 * static_cast<int>(clockwise) - 1;
    bool first_try = true;

    while (true)
    {
        int next_direction = (direction + type + DIRECTION_COUNT) % DIRECTION_COUNT;
        direction = static_cast<Direction>(next_direction);
        auto [delta_x, delta_y] = getDirDelta();
        if (auto next_pos = sprite_pos.cellPos(delta_x, delta_y);
            !game_map.isStone(next_pos) && game_map.getBlock(next_pos) != GameMap::Cell::TRAPDOOR)
            break;
        if (first_try)
        {
            type = -type;
            first_try = false;
        }
    }

}

