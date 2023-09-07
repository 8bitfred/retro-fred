#include "Rat.hpp"
#include "Game.hpp"

void Rat::update(Game &game, unsigned)
{
    // cx_limit is 0 if we are moving left, or 2 if we are moving right
    int cx_limit = direction + 1;
    if (sprite_pos.cx() == cx_limit)
    {
        auto const &game_map = game.getGameMap();
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

Sprite::RenderInfo const &Rat::getTexture() const
{
    // Offset of cells 0, 1, 2 and 3:
    static constexpr int c0 = 1;
    static constexpr int c1 = c0 + 26;
    static constexpr int c2 = c1 + 26;
    static constexpr int c3 = c2 + 26;
    static RenderInfo textures[2][2] =
        {
            {
                {TextureID::RAT, {c0, 8, 16, 10}, 0, 1},  // frame id 0
                {TextureID::RAT, {c1, 8, 16, 10}, 0, 1},  // frame id 1
            },
            {
                {TextureID::RAT, {c2, 8, 16, 10}, 0, 1},  // frame id 0
                {TextureID::RAT, {c3, 8, 16, 10}, 0, 1},  // frame id 1
            },
        };
    int dir_index = (direction + 1) >> 1;
    return textures[dir_index][alternate_frame];
}
