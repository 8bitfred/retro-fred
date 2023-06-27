#include "Fred.hpp"
#include "Actions.hpp"
#include "GameMap.hpp"
#include <iostream>

Fred::Fred(Frame const &frame, MapPos initial_position)
: Sprite(frame, initial_position, 4, 4)
{

}

std::pair<TextureID, Sprite::CenterPos> Fred::getTexture() const
{
    static std::pair<TextureID, Sprite::CenterPos> textures[2][7] =
        {
            {
                {TextureID::FRED_LEFT_STANDING, {}},
                {TextureID::FRED_LEFT_BIG_STEP, {}},
                {TextureID::FRED_LEFT_SMALL_STEP, {}},
                {TextureID::FRED_LEFT_CLIMBING1, {}},
                {TextureID::FRED_LEFT_CLIMBING2, {}},
                {TextureID::FRED_LEFT_SHOOTING, {}},
                {TextureID::FRED_LEFT_JUMP_SHOOTING, {}},
            },
            {
                {TextureID::FRED_RIGHT_STANDING, {}},
                {TextureID::FRED_RIGHT_BIG_STEP, {}},
                {TextureID::FRED_RIGHT_SMALL_STEP, {}},
                {TextureID::FRED_RIGHT_CLIMBING1, {}},
                {TextureID::FRED_RIGHT_CLIMBING2, {}},
                {TextureID::FRED_RIGHT_SHOOTING, {}},
                {TextureID::FRED_RIGHT_JUMP_SHOOTING, {}},
            },
        };
    return textures[static_cast<int>(direction)][static_cast<int>(type)];
}


std::pair<int, int> Fred::updateFred(GameMap const &game_map, unsigned action)
{
    if (type == Type::STANDING)
        return updateRestOnFoot(game_map, action);
    else if (type == Type::SMALL_STEP)
        return updateWalking(game_map, action);
    else if (type == Type::BIG_STEP) {
        if (sSpritePos.cy == 1)
            return updateWalking(game_map, action);
    }
    return {0, 0};
}

std::pair<int, int> Fred::updateRestOnFoot(GameMap const &game_map, unsigned action)
{
    int delta = 2 * static_cast<int>(direction) - 1;
    unsigned same_action = direction == Direction::LEFT ? ACTION_LEFT : ACTION_RIGHT;
    unsigned opposite_action = direction == Direction::LEFT ? ACTION_RIGHT : ACTION_LEFT;
    if ((action & opposite_action) != 0)
        direction = static_cast<Direction>(1 - static_cast<int>(direction));
    else if ((action && same_action) != 0)
    {
        if (sSpritePos.cx == 0)
        {
            auto next_cell = game_map.getCell(nextCellPos());
            if (next_cell == GameMap::Cell::empty ||
                next_cell == GameMap::Cell::rope_end)
            {
                sSpritePos.xadd(delta);
                type = Type::BIG_STEP;
                return {delta, 0};
            }
        }
        else if (sSpritePos.cx == 2) {
            sSpritePos.xadd(delta);
            type = Type::SMALL_STEP;
            return {delta, 0};
        }
    }
    return {0, 0};
}

std::pair<int, int> Fred::updateWalking(GameMap const &game_map, unsigned /* action */)
{
    int delta = 2 * static_cast<int>(direction) - 1;
    sSpritePos.xadd(delta);
    type = Type::STANDING;
    return {delta, 0};
}
