#include "Fred.hpp"
#include "Actions.hpp"
#include "GameMap.hpp"

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
    int dir_index = (static_cast<int>(frame_dir) + 1) >> 1;
    return textures[dir_index][static_cast<int>(frame_type)];
}

std::pair<int, int> Fred::updateFred(GameMap const &game_map, unsigned action)
{
    switch (state)
    {
    case State::REST_ON_FOOT:
        return stateRestOnFoot(game_map, action);
    case State::WALK:
        return stateWalk(game_map, action);
    case State::VERTICAL_JUMP:
        return stateVerticalJump(game_map, action);
    case State::SIDE_JUMP:
        return stateSideJump(game_map, action);
    case State::REST_ON_THE_ROPE:
        return stateRestOnTheRope(game_map, action);
    case State::ROPE_CLIMB:
        return stateRopeClimb(game_map, action);
    }
    return {0, 0};
}

std::pair<int, int> Fred::stateRestOnFoot(GameMap const &game_map, unsigned action)
{
    if (action != 0)
        return checkWalkActions(game_map, action);
    frame_type = FrameType::STANDING;
    return {0, 0};
}

std::pair<int, int> Fred::stateWalk(GameMap const &game_map, unsigned action)
{
    return checkWalkActions(game_map, action);
}

std::pair<int, int> Fred::checkWalkActions(GameMap const &game_map, unsigned action)
{
    int xdelta = static_cast<int>(frame_dir);
    unsigned action_same_dir = frame_dir == FrameDir::LEFT ? ACTION_LEFT : ACTION_RIGHT;
    unsigned action_opp_dir = frame_dir == FrameDir::LEFT ? ACTION_RIGHT : ACTION_LEFT;
    if ((action & action_opp_dir) != 0) {
        frame_dir = static_cast<FrameDir>(-static_cast<int>(frame_dir));
        frame_type = FrameType::STANDING;
        state = State::REST_ON_FOOT;
        return {0, 0};
    }
    else if ((action & action_same_dir) != 0) {
        if (sprite_pos.cx == 0) {
            if (game_map.isStone(nextCellPos()))
            {
                frame_type = FrameType::STANDING;
                state = State::REST_ON_FOOT;
                return {0, 0};
            }
            else if (auto next_cell = game_map.getCell(nextCellPos().vmove(1));
                     next_cell == GameMap::Cell::ROPE_MAIN)
            {
                frame_type = FrameType::BIG_STEP;
                sprite_pos.xadd(xdelta);
                sprite_pos.yadd(-1);
                state = State::SIDE_JUMP;
                jump_stage = 3;
                return {xdelta, 0};
            }
        }
        if (frame_type == FrameType::STANDING) 
        {
            if (sprite_pos.cx > 1)
                frame_type = FrameType::BIG_STEP;
            else
                frame_type = FrameType::SMALL_STEP;
        }
        else
            frame_type = FrameType::STANDING;
        sprite_pos.xadd(xdelta);
        state = State::WALK;
        return {xdelta, 0};
    }
    else if ((action & ACTION_UP) != 0) 
    {
        frame_type = FrameType::BIG_STEP;
        sprite_pos.yadd(-1);
        state = State::VERTICAL_JUMP;
        jump_stage = 3;
        return {0, 0};
    }
    else if ((action & ACTION_FIRE) != 0)
    {
        frame_type = FrameType::SHOOTING;
        state = State::REST_ON_FOOT;
        return {0, 0};
    }
    frame_type = FrameType::STANDING;
    state = State::REST_ON_FOOT;
    return {0, 0};
}

std::pair<int, int> Fred::stateVerticalJump(GameMap const &game_map, unsigned action)
{
    --jump_stage;
    if (jump_stage == 0) {
        if (sprite_pos.cx == 0 && 
            game_map.getCell(sprite_pos.cellPos()) == GameMap::Cell::ROPE_END) {
            frame_type = FrameType::CLIMBING1;
            state = State::REST_ON_THE_ROPE;
        }
        else {
            sprite_pos.yadd(1);
            if ((action & ACTION_FIRE) != 0)
                frame_type = FrameType::SHOOTING;
            else
                frame_type = FrameType::STANDING;
            state = State::REST_ON_FOOT;
        }
    }
    else 
    {
        if ((action & ACTION_FIRE) != 0)
            frame_type = FrameType::JUMP_SHOOTING;
        else
            frame_type = FrameType::BIG_STEP;
    }
    return {0, 0};
}

std::pair<int, int> Fred::stateSideJump(GameMap const &game_map, unsigned action)
{
    int xdelta = static_cast<int>(frame_dir);
    --jump_stage;
    sprite_pos.xadd(xdelta);
    if (jump_stage == 0) 
    {
        if (game_map.getCell(sprite_pos.cellPos()) == GameMap::Cell::EMPTY)
        {
            sprite_pos.yadd(1);
            if ((action & ACTION_FIRE) != 0)
                frame_type = FrameType::SHOOTING;
            else
                frame_type = FrameType::STANDING;
            state = State::REST_ON_FOOT;
        }
        else 
        {
            frame_type = FrameType::CLIMBING1;
            state = State::REST_ON_THE_ROPE;
        }
    }
    else 
    {
        if ((action & ACTION_FIRE) != 0)
            frame_type = FrameType::JUMP_SHOOTING;
        else
            frame_type = FrameType::BIG_STEP;
    }
    return {xdelta, 0};
}

std::pair<int, int> Fred::stateRestOnTheRope(GameMap const &game_map, unsigned action)
{
    if (action != 0)
        return checkRopeActions(game_map, action);
    return {0, 0};
}

std::pair<int, int> Fred::stateRopeClimb(GameMap const &game_map, unsigned action)
{
    return checkRopeActions(game_map, action);
}

std::pair<int, int> Fred::checkRopeActions(GameMap const &game_map, unsigned action)
{
    int xdelta = static_cast<int>(frame_dir);
    unsigned action_same_dir = frame_dir == FrameDir::LEFT ? ACTION_LEFT : ACTION_RIGHT;
    unsigned action_opp_dir = frame_dir == FrameDir::LEFT ? ACTION_RIGHT : ACTION_LEFT;
    if ((action & action_same_dir) != 0)
    {
        frame_dir = static_cast<FrameDir>(-static_cast<int>(frame_dir));
        state = State::REST_ON_THE_ROPE;
        return {0, 0};
    }
    else if ((action & action_opp_dir) != 0 &&
             sprite_pos.cy == 0 &&
             !game_map.isStone(sprite_pos.cellPos().hmove(-xdelta)))
    {
        frame_dir = static_cast<FrameDir>(-static_cast<int>(frame_dir));
        frame_type = FrameType::BIG_STEP;
        sprite_pos.xadd(-xdelta);
        state = State::SIDE_JUMP;
        jump_stage = 3;
        return {-xdelta, 0};
    }
    else if ((action & (ACTION_UP | ACTION_DOWN)) != 0)
    {
        int ydelta = (action & ACTION_UP) != 0 ? -1 : 1;
        if (sprite_pos.cy == 0)
        {
            auto next_pos = sprite_pos.cellPos().vmove(ydelta);
            if (game_map.isStone(next_pos))
            {
                state = State::REST_ON_THE_ROPE;
                return {0, 0};
            }
            else if (game_map.getCell(next_pos) == GameMap::Cell::TRAPDOOR)
            {
                // TODO: maze exit;
                return {0, 0};
            }
        }
        frame_type = frame_type == FrameType::CLIMBING1 ? FrameType::CLIMBING2 : FrameType::CLIMBING1;
        sprite_pos.yadd(ydelta);
        state = State::ROPE_CLIMB;
        return {0, ydelta};
    }
    state = State::REST_ON_THE_ROPE;
    return {0, 0};
}
