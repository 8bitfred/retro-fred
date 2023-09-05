#include "Ghost.hpp"
#include "Game.hpp"

Ghost::Ghost(Window const &window, MapPos const &pos, std::minstd_rand &random_engine)
    : Sprite::Sprite(window, pos, 3, 4), random_engine(random_engine)
{
    setRandomDirection();
}

void Ghost::update(Game &game, unsigned)
{
    alternate_frame ^= 1;
    bool char_limit = false;
    bool map_limit = false;
    bool is_stone = false;
    if (direction.x > 0)
    {
        char_limit = sprite_pos.cx() == 1;
        map_limit = sprite_pos.x() == (game.getGameMap().getWidth() - 2);
        is_stone = game.getGameMap().isStone(sprite_pos.cellPos(), 1);
    }
    else if (direction.x < 0)
    {
        char_limit = sprite_pos.cx() == 0;
        map_limit = sprite_pos.x() == 1;
        is_stone = game.getGameMap().isStone(sprite_pos.cellPos(), -1);
    }
    else if (direction.y > 0)
    {
        char_limit = sprite_pos.cy() == 1;
        map_limit = sprite_pos.y() == (game.getGameMap().getHeight() - 2);
        is_stone = game.getGameMap().isStone(sprite_pos.cellPos(), 0, 1);
    }
    else
    {
        char_limit = sprite_pos.cy() == 1;
        map_limit = sprite_pos.y() == 1;
        is_stone = game.getGameMap().isStone(sprite_pos.cellPos(), 0, -1);
    }

    if (char_limit)
    {
        if (map_limit) 
        {
            setRandomDirection();
            return;
        }
        else if (is_stone)
        {
            std::uniform_int_distribution<> distrib(0, 7);
            if (distrib(random_engine) <= 1)
            {
                setRandomDirection();
                return;
            }
        }
    }
    sprite_pos.xadd(direction.x);
    sprite_pos.yadd(direction.y);
}

Sprite::RenderInfo Ghost::getTexture() const
{
    // Offset of cells 0, 1, 2 and 3:
    static constexpr int c0 = 8;
    static constexpr int c1 = c0 + 50;
    static constexpr int c2 = c1 + 50;
    static constexpr int c3 = c2 + 50;
    static RenderInfo textures[2][2] =
        {
            {
                {TextureID::GHOST, {c0, 1, 26, 33}, 1, 0},  // frame id 0
                {TextureID::GHOST, {c1, 1, 26, 33}, 1, 0},  // frame id 1
            },
            {
                {TextureID::GHOST, {c2, 1, 26, 33}, 1, 0},  // frame id 0
                {TextureID::GHOST, {c3, 1, 26, 33}, 1, 0},  // frame id 1
            },
        };
    // Note that only one of velocity.x and velocity.y is not 0 at a given time
    int frame_dir = direction.x + direction.y;
    int dir_index = (frame_dir + 1) >> 1;
    return textures[dir_index][alternate_frame];
}

void Ghost::setRandomDirection()
{
    static Direction directions[] = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1},
    };
    std::uniform_int_distribution<> distrib(0, 3);
    while (true) {
        int choice = distrib(random_engine);
        if (direction != directions[choice]) {
            direction = directions[choice];
            break;
        }
    }
}
