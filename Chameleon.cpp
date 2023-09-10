#include "Chameleon.hpp"
#include "Game.hpp"

void Chameleon::update(Game &game, unsigned)
{
    alternate_frame ^= 1;
    if (sprite_pos.cx() == 1 || sprite_pos.cx() == 2)
    {
        sprite_pos.xadd(direction.x);
        return;
    }
    std::uniform_int_distribution<> distrib(0, 63);
    if (distrib(random_engine) == 3)
    {
        direction.x = -direction.x;
        sprite_pos.xadd(direction.x);
        return;
    }
    int limit = direction.y < 0 ? 0 : 3;
    if (sprite_pos.cy() == limit && 
        !isValidCell(game.getGameMap(), sprite_pos.cellPos(0, direction.y)))
    {
        direction.y = -direction.y;
        return;
    }
    sprite_pos.yadd(direction.y);
}

bool Chameleon::isValidCell(GameMap const &game_map, CellPos const &pos)
{
    return !game_map.isStone(pos) &&
        game_map.isStone(pos, -1) &&
        game_map.isStone(pos, 1);
}

Sprite::BoxParams const &Chameleon::getBoxParams() const
{
    static BoxParams box_params[2][2][2] = {
        {
            {
                {      1, 1, {0, 0, 8, 16}, {} },
                {   18+1, 1, {0, 0, 8, 16}, {} },
            },
            {
                { 2*18+1, 1, {0, 0, 8, 16}, {} },
                { 3*18+1, 1, {0, 0, 8, 16}, {} },
            },
        },
        {
            {
                { 4*18+1, 1, {0, 0, 8, 16}, {} },
                { 5*18+1, 1, {0, 0, 8, 16}, {} },
            },
            {
                { 6*18+1, 1, {0, 0, 8, 16}, {} },
                { 7*18+1, 1, {0, 0, 8, 16}, {} },
            },
        },
    };
    // side is 0 if cx is 0 or 2, and 1 if cx is 1 or 3
    int side = sprite_pos.cx() % 2;
    // vdir is 0 if direction.y is -1 and 1 if direction.y is +1
    int vdir = (direction.y + 1) >> 1;
    return box_params[side][vdir][alternate_frame];
}

Sprite::RenderParams Chameleon::getRenderParams() const
{
    return {TextureID::CHAMELEON, false, {}};
}
