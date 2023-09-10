#include "Vampire.hpp"
#include "Game.hpp"

Vampire::Vampire(MapPos const &pos, std::minstd_rand &random_engine)
    : MultiDirEnemy::MultiDirEnemy(pos, random_engine)
{
    std::uniform_int_distribution<> distrib(0, DIRECTION_COUNT - 1);
    direction = static_cast<Direction>(distrib(random_engine));
}

void Vampire::update(Game &game, unsigned)
{
    alternate_frame ^= 1;
    if (sprite_pos.cx() == 0 && sprite_pos.cy() == 0)
        selectDirection(game.getGameMap(), getRandomSense());
    if (state == State::SLOW)
        stateSlow(game);
    else if (state == State::STATIC)
        stateStatic(game);
    else if (state == State::FAST)
        stateFast(game);
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
    if (sprite_pos.cx() == 0 && sprite_pos.cy() == 0)
        selectDirection(game.getGameMap(), getRandomSense());
    auto [delta_x2, delta_y2] = getDirDelta();
    sprite_pos.xadd(delta_x2);
    sprite_pos.yadd(delta_y2);
}
