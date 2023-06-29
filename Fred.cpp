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
                {TextureID::FRED_LEFT_SHOOTING, {8, 0}},
                {TextureID::FRED_LEFT_JUMP_SHOOTING, {8, 0}},
            },
            {
                {TextureID::FRED_RIGHT_STANDING, {}},
                {TextureID::FRED_RIGHT_BIG_STEP, {}},
                {TextureID::FRED_RIGHT_SMALL_STEP, {}},
                {TextureID::FRED_RIGHT_CLIMBING1, {}},
                {TextureID::FRED_RIGHT_CLIMBING2, {}},
                {TextureID::FRED_RIGHT_SHOOTING, {0, 0}},
                {TextureID::FRED_RIGHT_JUMP_SHOOTING, {0, 0}},
            },
        };
    return textures[static_cast<int>(direction)][static_cast<int>(type)];
}

std::pair<int, int> Fred::updateFred(GameMap const &game_map, unsigned action)
{
    switch (sState)
    {
    case State::REST_ON_FOOT:
        return updateRestOnFoot(game_map, action);
    case State::WALK:
        return updateWalk(game_map, action);
    case State::VERTICAL_JUMP:
        return updateVerticalJump(game_map, action);
    case State::SIDE_JUMP:
        return updateSideJump(game_map, action);
    case State::REST_ON_THE_ROPE:
        return updateRestOnTheRope(game_map, action);
    case State::ROPE_CLIMB:
        return updateRopeClimb(game_map, action);
    }
    return {0, 0};
}

std::pair<int, int> Fred::updateRestOnFoot(GameMap const &game_map, unsigned action)
{
    if (action != 0)
        return updateWalk(game_map, action);
    type = Type::STANDING;
    return {0, 0};
}

std::pair<int, int> Fred::updateWalk(GameMap const &game_map, unsigned action)
{
    int xdelta = 2 * static_cast<int>(direction) - 1;
    unsigned action_same_dir = direction == Direction::LEFT ? ACTION_LEFT : ACTION_RIGHT;
    unsigned action_opp_dir = direction == Direction::LEFT ? ACTION_RIGHT : ACTION_LEFT;
    if ((action & action_opp_dir) != 0) {
        direction = static_cast<Direction>(1 - static_cast<int>(direction));
        type = Type::STANDING;
        sState = State::REST_ON_FOOT;
        return {0, 0};
    }
    else if ((action & action_same_dir) != 0) {
        if (sSpritePos.cx == 0) {
            if (auto next_cell = game_map.getCell(nextCellPos());
                next_cell == GameMap::Cell::stone1 ||
                next_cell == GameMap::Cell::stone2 ||
                next_cell == GameMap::Cell::stone3)
            {
                type = Type::STANDING;
                sState = State::REST_ON_FOOT;
                return {0, 0};
            }
            else if (auto next_cell = game_map.getCell(nextCellPos().vmove(1));
                     next_cell == GameMap::Cell::rope_middle)
            {
                type = Type::BIG_STEP;
                sSpritePos.xadd(xdelta);
                sSpritePos.yadd(-1);
                sState = State::SIDE_JUMP;
                sStage = 3;
                return {xdelta, 0};
            }
        }
        if (type == Type::STANDING) 
        {
            if (sSpritePos.cx > 1)
                type = Type::BIG_STEP;
            else
                type = Type::SMALL_STEP;
        }
        else
            type = Type::STANDING;
        sSpritePos.xadd(xdelta);
        sState = State::WALK;
        return {xdelta, 0};
    }
    else if ((action & ACTION_UP) != 0) 
    {
        type = Type::BIG_STEP;
        sSpritePos.yadd(-1);
        sState = State::VERTICAL_JUMP;
        sStage = 3;
        return {0, 0};
    }
    else if ((action & ACTION_FIRE) != 0)
    {
        type = Type::SHOOTING;
        sState = State::REST_ON_FOOT;
        return {0, 0};
    }
    type = Type::STANDING;
    sState = State::REST_ON_FOOT;
    return {0, 0};
}

std::pair<int, int> Fred::updateVerticalJump(GameMap const &game_map, unsigned action)
{
    --sStage;
    if (sStage == 0) {
        if (sSpritePos.cx == 0 && 
            game_map.getCell(sSpritePos.cellPos()) == GameMap::Cell::rope_end) {
            type = Type::CLIMBING1;
            sState = State::REST_ON_THE_ROPE;
        }
        else {
            sSpritePos.yadd(1);
            if ((action & ACTION_FIRE) != 0)
                type = Type::SHOOTING;
            else
                type = Type::STANDING;
            sState = State::REST_ON_FOOT;
        }
    }
    else 
    {
        if ((action & ACTION_FIRE) != 0)
            type = Type::JUMP_SHOOTING;
        else
            type = Type::BIG_STEP;
    }
    return {0, 0};
}

std::pair<int, int> Fred::updateSideJump(GameMap const &game_map, unsigned action)
{
    int xdelta = 2 * static_cast<int>(direction) - 1;
    --sStage;
    sSpritePos.xadd(xdelta);
    if (sStage == 0) 
    {
        if (game_map.getCell(sSpritePos.cellPos()) == GameMap::Cell::empty)
        {
            sSpritePos.yadd(1);
            if ((action & ACTION_FIRE) != 0)
                type = Type::SHOOTING;
            else
                type = Type::STANDING;
            sState = State::REST_ON_FOOT;
        }
        else 
        {
            type = Type::CLIMBING1;
            sState = State::REST_ON_THE_ROPE;
        }
    }
    else 
    {
        if ((action & ACTION_FIRE) != 0)
            type = Type::JUMP_SHOOTING;
        else
            type = Type::BIG_STEP;
    }
    return {xdelta, 0};
}

std::pair<int, int> Fred::updateRestOnTheRope(GameMap const &game_map, unsigned action)
{
    if (action != 0)
        return updateRopeClimb(game_map, action);
    return {0, 0};
}

std::pair<int, int> Fred::updateRopeClimb(GameMap const &game_map, unsigned action)
{
    int xdelta = 2 * static_cast<int>(direction) - 1;
    unsigned action_same_dir = direction == Direction::LEFT ? ACTION_LEFT : ACTION_RIGHT;
    unsigned action_opp_dir = direction == Direction::LEFT ? ACTION_RIGHT : ACTION_LEFT;
    if ((action & action_same_dir) != 0)
    {
        direction = static_cast<Direction>(1 - static_cast<int>(direction));
        sState = State::REST_ON_THE_ROPE;
        return {0, 0};
    }
    else if (auto next_cell = game_map.getCell(sSpritePos.cellPos().hmove(-xdelta)); 
             (action & action_opp_dir) != 0 &&
             sSpritePos.cy == 0 &&
             next_cell == GameMap::Cell::empty || next_cell == GameMap::Cell::rope_end)
    {
        direction = static_cast<Direction>(1 - static_cast<int>(direction));
        type = Type::BIG_STEP;
        sSpritePos.xadd(-xdelta);
        sState = State::SIDE_JUMP;
        sStage = 3;
        return {-xdelta, 0};
    }
    else if ((action & (ACTION_UP | ACTION_DOWN)) != 0)
    {
        int ydelta = (action & ACTION_UP) != 0 ? -1 : 1;
        if (sSpritePos.cy == 0)
        {
            auto next_cell = game_map.getCell(sSpritePos.cellPos().vmove(ydelta));
            if (next_cell == GameMap::Cell::stone1 ||
                next_cell == GameMap::Cell::stone2 ||
                next_cell == GameMap::Cell::stone3)
            {
                sState = State::REST_ON_THE_ROPE;
                return {0, 0};
            }
            else if (next_cell == GameMap::Cell::trapdoor)
            {
                // TODO: maze exit;
                return {0, 0};
            }
        }
        type = type == Type::CLIMBING1 ? Type::CLIMBING2 : Type::CLIMBING1;
        sSpritePos.yadd(ydelta);
        sState = State::ROPE_CLIMB;
        return {0, ydelta};
    }
    sState = State::REST_ON_THE_ROPE;
    return {0, 0};
}
