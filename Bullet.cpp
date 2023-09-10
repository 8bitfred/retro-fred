#include "Bullet.hpp"
#include "Game.hpp"

bool Bullet::isAlive(Game &game)
{
    auto const &fred_pos = game.getFredPos();
    if (game.getGameMap().isStone(sprite_pos.cellPos()))
        return false;
    int distance;
    if (direction > 0)
        distance = sprite_pos.getCharX() - fred_pos.getCharX() - 4;
    else
        distance = fred_pos.getCharX() - 3 - sprite_pos.getCharX();
    return distance < MAX_DISTANCE;
}

Sprite::BoxParams const &Bullet::getBoxParams() const
{
    static BoxParams box_params = {1, 1, {0, 0, 24, 8}, {}};
    return box_params;
}

Sprite::RenderParams Bullet::getRenderParams() const
{
    return {TextureID::BULLET, false, {}};
}
