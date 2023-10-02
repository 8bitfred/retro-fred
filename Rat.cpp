#include "Rat.hpp"
#include "GameMap.hpp"

void Rat::update(unsigned)
{
    // cx_limit is 0 if we are moving left, or 2 if we are moving right
    int cx_limit = direction + 1;
    if (sprite_pos.cx() == cx_limit)
    {
        if (auto cell_pos = sprite_pos.cellPos();
            game_map.isStone(cell_pos, direction) || !game_map.isStone(cell_pos, direction, 1))
            direction = -direction;
        else
            sprite_pos.xadd(direction);
    }
    else
        sprite_pos.xadd(direction);
    alternate_frame = alternate_frame ^ 1;
}

Sprite::BoxParams const &Rat::getBoxParams() const
{
    static BoxParams box_params[2] = {
        {    1, 9, {0, -1, 16, 10}, {{2, 1, 12, 8}} },
        { 26+1, 9, {0, -1, 16, 10}, {{2, 1, 12, 8}} },
    };
    return box_params[alternate_frame];
}

Sprite::RenderParams Rat::getRenderParams() const
{
    return {TextureID::RAT, direction == 1, {}};
}
