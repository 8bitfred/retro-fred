#include "Vampire.hpp"
#include "Game.hpp"

Vampire::Vampire(MapPos const &pos, std::minstd_rand &random_engine)
    : MultiDirEnemy::MultiDirEnemy(pos, random_engine)
{
    std::uniform_int_distribution<> distrib(0, DIRECTION_COUNT - 1);
    direction = static_cast<Direction>(distrib(random_engine));
    std::uniform_int_distribution<> distrib_frame(0, 1);
    alternate_frame = distrib_frame(random_engine);
}

void Vampire::update(Game &game, unsigned)
{
    if (sprite_pos.cx() == 0 && sprite_pos.cy() == 0)
        selectDirection(game.getGameMap(), getRandomSense());
    if (mode == Mode::SLOW)
        moveSlow(game);
    else if (mode == Mode::STATIC)
        moveStatic(game);
    else if (mode == Mode::FAST)
        moveFast(game);
    alternate_frame ^= 1;
}

Sprite::BoxParams const &Vampire::getBoxParams() const
{
    static BoxParams box_params[] = {
        {    9, 9, { -1, -1, 26, 18}, {{0, 0, 24, 12}}},
        { 50+9, 9, { -1, -1, 26, 18}, {{2, 3, 20, 13}}},
    };
    return box_params[alternate_frame];
}

Sprite::RenderParams Vampire::getRenderParams() const
{
    // Note that only one of dir_x and dir_y is not 0 at a given time
    auto [dir_x, dir_y] = getDirDelta();
    int frame_dir = dir_x + dir_y;
    return {TextureID::VAMPIRE, frame_dir == 1, {}};
}



void Vampire::moveSlow(Game &)
{
    std::uniform_int_distribution<> distrib(0, 15);
    int n = distrib(random_engine);
    if (n == 6)
        mode = Mode::STATIC;
    else if (n == 3)
        mode = Mode::FAST;
    auto [delta_x, delta_y] = getDirDelta();
    sprite_pos.xadd(delta_x);
    sprite_pos.yadd(delta_y);
}

void Vampire::moveStatic(Game &)
{
    std::uniform_int_distribution<> distrib1(0, 3);
    int n = distrib1(random_engine);
    if (n == 0) {
        mode = Mode::SLOW;
        std::uniform_int_distribution<> distrib2(0, 1);
        int m = distrib2(random_engine);
        if (m == 0)
            direction = static_cast<Direction>((direction + 2) % DIRECTION_COUNT);
    }
}

void Vampire::moveFast(Game &game)
{
    std::uniform_int_distribution<> distrib(0, 7);
    if (distrib(random_engine) == 0)
        mode = Mode::SLOW;
    auto [delta_x1, delta_y1] = getDirDelta();
    sprite_pos.xadd(delta_x1);
    sprite_pos.yadd(delta_y1);
    if (sprite_pos.cx() == 0 && sprite_pos.cy() == 0)
        selectDirection(game.getGameMap(), getRandomSense());
    auto [delta_x2, delta_y2] = getDirDelta();
    sprite_pos.xadd(delta_x2);
    sprite_pos.yadd(delta_y2);
}
