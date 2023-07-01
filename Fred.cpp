#include "Fred.hpp"
#include "Game.hpp"
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

void Fred::updateFred(Game& game, unsigned events)
{
    switch (state)
    {
    case State::REST_ON_FOOT:
        stateRestOnFoot(game, events);
        break;
    case State::WALK:
        stateWalk(game, events);
        break;
    case State::VERTICAL_JUMP:
        stateVerticalJump(game, events);
        break;
    case State::SIDE_JUMP:
        stateSideJump(game, events);
        break;
    case State::REST_ON_THE_ROPE:
        stateRestOnTheRope(game, events);
        break;
    case State::ROPE_CLIMB:
        stateRopeClimb(game, events);
        break;
    }
}

void Fred::stateRestOnFoot(Game& game, unsigned events)
{
    if (events != 0) {
        checkWalkActions(game, events);
        return;
    }
    frame_type = FrameType::STANDING;
}

void Fred::stateWalk(Game& game, unsigned events)
{
    checkWalkActions(game, events);
}

void Fred::checkWalkActions(Game& game, unsigned events)
{
    int xdelta = static_cast<int>(frame_dir);
    unsigned events_same_dir = frame_dir == FrameDir::LEFT ? Game::EVENT_LEFT : Game::EVENT_RIGHT;
    unsigned events_opp_dir = frame_dir == FrameDir::LEFT ? Game::EVENT_RIGHT : Game::EVENT_LEFT;
    if ((events & events_opp_dir) != 0) {
        frame_dir = static_cast<FrameDir>(-static_cast<int>(frame_dir));
        frame_type = FrameType::STANDING;
        state = State::REST_ON_FOOT;
        return;
    }
    else if ((events & events_same_dir) != 0) {
        if (sprite_pos.cx == 0) {
            if (game.getGameMap().isStone(nextCellPos()))
            {
                frame_type = FrameType::STANDING;
                state = State::REST_ON_FOOT;
                return;
            }
            else if (auto next_cell = game.getGameMap().getCell(nextCellPos().vmove(1));
                     next_cell == GameMap::Cell::ROPE_MAIN)
            {
                frame_type = FrameType::BIG_STEP;
                sprite_pos.xadd(xdelta);
                sprite_pos.yadd(-1);
                state = State::SIDE_JUMP;
                jump_stage = 3;
                game.moveFrame(xdelta, 0);
                return;
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
        game.moveFrame(xdelta, 0);
        return;
    }
    else if ((events & Game::EVENT_UP) != 0) 
    {
        frame_type = FrameType::BIG_STEP;
        sprite_pos.yadd(-1);
        state = State::VERTICAL_JUMP;
        jump_stage = 3;
        return;
    }
    else if ((events & Game::EVENT_FIRE) != 0)
    {
        frame_type = FrameType::SHOOTING;
        state = State::REST_ON_FOOT;
        return;
    }
    frame_type = FrameType::STANDING;
    state = State::REST_ON_FOOT;
}

void Fred::stateVerticalJump(Game& game, unsigned events)
{
    --jump_stage;
    if (jump_stage == 0) {
        if (sprite_pos.cx == 0 && 
            game.getGameMap().getCell(sprite_pos.cellPos()) == GameMap::Cell::ROPE_END) {
            frame_type = FrameType::CLIMBING1;
            state = State::REST_ON_THE_ROPE;
        }
        else {
            sprite_pos.yadd(1);
            if ((events & Game::EVENT_FIRE) != 0)
                frame_type = FrameType::SHOOTING;
            else
                frame_type = FrameType::STANDING;
            state = State::REST_ON_FOOT;
        }
    }
    else 
    {
        if ((events & Game::EVENT_FIRE) != 0)
            frame_type = FrameType::JUMP_SHOOTING;
        else
            frame_type = FrameType::BIG_STEP;
    }
}

void Fred::stateSideJump(Game& game, unsigned events)
{
    int xdelta = static_cast<int>(frame_dir);
    --jump_stage;
    sprite_pos.xadd(xdelta);
    if (jump_stage == 0) 
    {
        if (game.getGameMap().getCell(sprite_pos.cellPos()) == GameMap::Cell::EMPTY)
        {
            sprite_pos.yadd(1);
            if ((events & Game::EVENT_FIRE) != 0)
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
        if ((events & Game::EVENT_FIRE) != 0)
            frame_type = FrameType::JUMP_SHOOTING;
        else
            frame_type = FrameType::BIG_STEP;
    }
    game.moveFrame(xdelta, 0);
}

void Fred::stateRestOnTheRope(Game& game, unsigned events)
{
    if (events != 0)
        checkRopeActions(game, events);
}

void Fred::stateRopeClimb(Game& game, unsigned events)
{
    checkRopeActions(game, events);
}

void Fred::checkRopeActions(Game& game, unsigned events)
{
    int xdelta = static_cast<int>(frame_dir);
    unsigned events_same_dir = frame_dir == FrameDir::LEFT ? Game::EVENT_LEFT : Game::EVENT_RIGHT;
    unsigned events_opp_dir = frame_dir == FrameDir::LEFT ? Game::EVENT_RIGHT : Game::EVENT_LEFT;
    if ((events & events_same_dir) != 0)
    {
        frame_dir = static_cast<FrameDir>(-static_cast<int>(frame_dir));
        state = State::REST_ON_THE_ROPE;
        return;
    }
    else if ((events & events_opp_dir) != 0 &&
             sprite_pos.cy == 0 &&
             !game.getGameMap().isStone(sprite_pos.cellPos().hmove(-xdelta)))
    {
        frame_dir = static_cast<FrameDir>(-static_cast<int>(frame_dir));
        frame_type = FrameType::BIG_STEP;
        sprite_pos.xadd(-xdelta);
        state = State::SIDE_JUMP;
        jump_stage = 3;
        game.moveFrame(-xdelta, 0);
        return;
    }
    else if ((events & (Game::EVENT_UP | Game::EVENT_DOWN)) != 0)
    {
        int ydelta = (events & Game::EVENT_UP) != 0 ? -1 : 1;
        if (sprite_pos.cy == 0)
        {
            auto next_pos = sprite_pos.cellPos().vmove(ydelta);
            if (game.getGameMap().isStone(next_pos))
            {
                state = State::REST_ON_THE_ROPE;
                return;
            }
            else if (game.getGameMap().getCell(next_pos) == GameMap::Cell::TRAPDOOR)
            {
                // TODO: maze exit;
                return;
            }
        }
        frame_type = frame_type == FrameType::CLIMBING1 ? FrameType::CLIMBING2 : FrameType::CLIMBING1;
        sprite_pos.yadd(ydelta);
        state = State::ROPE_CLIMB;
        game.moveFrame(0, ydelta);
        return;
    }
    state = State::REST_ON_THE_ROPE;
}
