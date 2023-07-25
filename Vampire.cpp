#include "Vampire.hpp"
#include "Game.hpp"

Vampire::Vampire(Frame const &frame, MapPos const &pos, std::minstd_rand &random_engine)
    : Sprite::Sprite(frame, pos, 3, 2), random_engine(random_engine)
{
    std::uniform_int_distribution<> distrib(0, DIRECTION_COUNT - 1);
    direction = static_cast<Direction>(distrib(random_engine));
}

void Vampire::update(Game &game, unsigned)
{
    alternate_frame ^= 1;
    if (sprite_pos.cx == 0 && sprite_pos.cy == 0)
        changeDirection(game.getGameMap());
    if (state == State::SLOW)
        stateSlow(game);
    else if (state == State::STATIC)
        stateStatic(game);
    else if (state == State::FAST)
        stateFast(game);
}

Sprite::RenderInfo Vampire::getTexture() const
{
    // Offset of cells 0, 1, 2 and 3:
    static constexpr int c0 = 8;
    static constexpr int c1 = c0 + 50;
    static constexpr int c2 = c1 + 50;
    static constexpr int c3 = c2 + 50;
    static RenderInfo textures[2][2] =
        {
            {
                {TextureID::VAMPIRE, {c0, 8, 26, 14}, 1, 1},  // frame id 0
                {TextureID::VAMPIRE, {c1, 8, 26, 14}, 1, 1},  // frame id 0
            },
            {
                {TextureID::VAMPIRE, {c2, 8, 26, 14}, 1, 1},  // frame id 0
                {TextureID::VAMPIRE, {c3, 8, 26, 14}, 1, 1},  // frame id 0
            },
        };
    // Note that only one of dir_x and dir_y is not 0 at a given time
    auto [dir_x, dir_y] = getDirDelta();
    int frame_dir = dir_x + dir_y;
    int dir_index = (frame_dir + 1) >> 1;
    return textures[dir_index][alternate_frame];
}

std::pair<int, int> Vampire::getDirDelta() const
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

void Vampire::changeDirection(GameMap const& game_map)
{
    std::uniform_int_distribution<> distrib(0, 1);
    int type = 2 * distrib(random_engine) - 1;
    bool first_try = true;

    while (true)
    {
        int next_direction = (direction + type + DIRECTION_COUNT) % DIRECTION_COUNT;
        direction = static_cast<Direction>(next_direction);
        auto [delta_x, delta_y] = getDirDelta();
        if (!game_map.isStone(sprite_pos.cellPos().hmove(delta_x).vmove(delta_y)))
            break;
        if (first_try)
        {
            type = -type;
            first_try = false;
        }
    }
}

void Vampire::stateSlow(Game &)
{
    std::uniform_int_distribution<> distrib(0, 15);
    int n = distrib(random_engine);
    if (n == 6)
        state = State::STATIC;
    else if (n == 3)
        state = State::FAST;
    auto [delta_x, delta_y] = getDirDelta();
    sprite_pos.xadd(delta_x);
    sprite_pos.yadd(delta_y);
}

void Vampire::stateStatic(Game &)
{
    std::uniform_int_distribution<> distrib(0, 8);
    int n = distrib(random_engine);
    if (n < 2)
    {
        state = State::SLOW;
        if (n == 0)
            direction = static_cast<Direction>((direction + 2) % DIRECTION_COUNT);
    }
}

void Vampire::stateFast(Game &game)
{
    std::uniform_int_distribution<> distrib(0, 8);
    if (distrib(random_engine) == 0)
        state = State::SLOW;
    auto [delta_x1, delta_y1] = getDirDelta();
    sprite_pos.xadd(delta_x1);
    sprite_pos.yadd(delta_y1);
    if (sprite_pos.cx == 0 && sprite_pos.cy == 0)
        changeDirection(game.getGameMap());
    auto [delta_x2, delta_y2] = getDirDelta();
    sprite_pos.xadd(delta_x2);
    sprite_pos.yadd(delta_y2);
}
