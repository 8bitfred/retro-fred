#include "Fred.hpp"
#include "fredcore/Game.hpp"
#include "Object.hpp" // TODO: this should not be exposed here
#include <functional>

Sprite::BoxParams const &Fred::getBoxParams() const
{
    static BoxParams box_params[] = {
        {      9,      9, {-1, -1, 34, 34},       // STANDING
            {{ 9, 0, 14, 32}, { 0, 13, 26, 10}} },
        {   58+9,      9, {-1, -1, 34, 34},       // BIG_STEP
            {{ 9, 1, 14, 31}, { 0, 12, 32, 20}} },
        { 2*58+9,      9, {-1, -1, 34, 34},       // SMALL_STEP
            {{ 9, 1, 14, 31}, { 0, 15, 24,  8}, { 7, 24, 19, 8}} },
        { 3*58+9,      9, {-1, -1, 34, 34},       // CLIMBING_CLAMPING
            {{14, 1, 18, 23}, {10, 20,  9, 11}} },
        {      9,   58+9, {-1, -1, 34, 34},       // CLIMBING_EXTENDING
            {{13, 0, 19, 23}, {10, 21, 11, 11}} },
        {   58+9,   58+9, {-8, -1, 48, 34},       // SHOOTING, STANDING
            {{ 9, 0, 14, 32}, { 0,  9, 26, 14}} },
        { 2*58+9,   58+9, {-8, -1, 48, 34},       // SHOOTING, BIG_STEP
            {{ 9, 1, 14, 31}, { 0, 10, 32, 22}} },
        { 3*58+9,   58+9, {-8, -1, 48, 34},       // SHOOTING, SMALL_STEP
            {{ 9, 1, 14, 31}, { 0, 10, 24, 13}, { 7, 24, 19, 8}} },
    };
    static constexpr int SHOOTING_STANDING = 5;

    int frame_index = static_cast<int>(frame);
    if (shooting && frame_index <= static_cast<int>(Frame::SMALL_STEP))
        frame_index += SHOOTING_STANDING;
    return box_params[frame_index];
}

Sprite::RenderParams Fred::getRenderParams() const
{
    ColorModulation color_mods[] = {
        {255, 255,   0},  // yellow
        {  0, 255, 255},  // cyan
        {  0, 255,   0},  // green
        {255,   0, 255},  // magenta
        {255,   0,   0},  // red
        {  0,   0, 255},  // blue
        {  0,   0,   0},  // black
    };
    static_assert(std::size(color_mods) == static_cast<size_t>(Color::COUNT));
    return {TextureID::FRED, direction == 1, color_mods[static_cast<size_t>(color)]};
}

Label Fred::getLabel() const
{
    auto frame_index = std::min(static_cast<unsigned>(frame), 3u);
    auto dir_index = (direction + 1) >> 1;
    auto index = static_cast<unsigned>(LabelID::FRED_STANDING_LEFT) + 2 * frame_index + dir_index;
    if (!(static_cast<int>(LabelID::FRED_STANDING_LEFT) <= index &&
        index <= static_cast<int>(LabelID::FRED_CLIMBING_RIGHT)))
    {
        SDL_Log("frame_index=%d dir_index=%d index=%d", frame_index, dir_index, index);
    }
    assert(static_cast<int>(LabelID::FRED_STANDING_LEFT) <= index &&
           index <= static_cast<int>(LabelID::FRED_CLIMBING_RIGHT));
    auto label = labelOf(static_cast<LabelID>(index));
    if (shooting)
        label |= labelOf(LabelID::FRED_SHOOTING);
    return label;
}

void Fred::updateFred(EventMask event_mask)
{
    int input_x = 0, input_y = 0;
    bool input_fire = false;
    if (event_mask.check(GameEvent::UP))
        input_y = -1;
    else if (event_mask.check(GameEvent::DOWN))
        input_y = 1;
    else if (event_mask.check(GameEvent::LEFT))
        input_x = -1;
    else if (event_mask.check(GameEvent::RIGHT))
        input_x = 1;
    if (event_mask.check(GameEvent::FIRE))
        input_fire = true;

    switch (state)
    {
    case State::WALK:
        stateWalk(input_x, input_y);
        break;
    case State::VERTICAL_JUMP:
        stateVerticalJump(input_x, input_y);
        break;
    case State::SIDE_JUMP:
        stateSideJump(input_x, input_y);
        break;
    case State::CLIMB:
        stateRopeClimb(input_x, input_y);
        break;
    case State::EXIT_MAZE:
        stateExitMaze();
        break;
    case State::GAME_OVER:
        stateGameOver();
        break;
    default:
        break;
    }
    checkFire(input_fire);
    if (collision_timer > 0)
        --collision_timer;
    else
        color = Color::YELLOW;
    game.updateFredPos(sprite_pos, vposition);
}

void Fred::checkFire(bool input_fire)
{
    if (input_fire && game.canShoot() && state != State::CLIMB)
    {
        shooting = true;
        auto bullet_pos = sprite_pos;
        bullet_pos.yadd(1);
        if (direction > 0)
            bullet_pos.xadd(4);
        else
            bullet_pos.xadd(-3);
        game.fireGun(bullet_pos, direction);
    }
    else
        shooting = false;
}

void Fred::stateWalk(int input_x, int input_y)
{
    if (input_x == (-direction)) 
    {
        direction = -direction;
        frame = Frame::STANDING;
    }
    else if (input_x == direction)
        walkOneStep();
    else if (input_y < 0)
        startVerticalJump();
    else 
        frame = Frame::STANDING;
}

void Fred::walkOneStep()
{
    if (sprite_pos.cx() == 0)
    {
        if (game.getGameMap().isStone(sprite_pos.cellPos(), direction))
        {
            frame = Frame::STANDING;
            return;
        }
        else if (game.getGameMap().getBlock(sprite_pos.cellPos(), direction, 1) == GameMap::Cell::ROPE_MAIN)
        {
            sprite_pos.yadd(-1);
            vposition = 0;
            startSideJump();
            return;
        }
    }
    if (frame == Frame::STANDING)
    {
        if (sprite_pos.cx() > 1)
            frame = Frame::BIG_STEP;
        else
            frame = Frame::SMALL_STEP;
    }
    else
    {
        frame = Frame::STANDING;
        game.addSound(SoundID::WALK);
    }
    sprite_pos.xadd(direction);
}

void Fred::startSideJump()
{
    frame = Frame::BIG_STEP;
    state = State::SIDE_JUMP;
    jump_stage = 3;
    sprite_pos.xadd(direction);
    game.addSound(SoundID::JUMP);
}

void Fred::startVerticalJump()
{
    frame = Frame::BIG_STEP;
    state = State::VERTICAL_JUMP;
    jump_stage = 3;
    sprite_pos.yadd(-1);
    vposition = 0;
    game.addSound(SoundID::JUMP);
}

void Fred::stateVerticalJump(int, int)
{
    --jump_stage;
    if (jump_stage == 1 &&
        sprite_pos.cx() == 0)
    {
        if (game.getGameMap().getBlock(sprite_pos.cellPos(), 0, -1) == GameMap::Cell::TRAPDOOR)
        {
            frame = Frame::CLIMBING_CLAMPING;
            state = State::EXIT_MAZE;
        }
        else if (game.getGameMap().getBlock(sprite_pos.cellPos()) == GameMap::Cell::ROPE_END)
        {
            frame = Frame::CLIMBING_CLAMPING;
            state = State::CLIMB;
        }
    }
    else if (jump_stage == 0)
    {
        sprite_pos.yadd(1);
        vposition = 1;
        frame = Frame::STANDING;
        game.addSound(SoundID::WALK);
        state = State::WALK;
    }
}

void Fred::stateSideJump(int, int)
{
    --jump_stage;
    sprite_pos.xadd(direction);
    if (jump_stage == 0)
    {
        if (game.getGameMap().isStone(sprite_pos.cellPos(), 0, 1))
        {
            sprite_pos.yadd(1);
            vposition = 1;
            frame = Frame::STANDING;
            game.addSound(SoundID::WALK);
            state = State::WALK;
        }
        else 
        {
            frame = Frame::CLIMBING_CLAMPING;
            state = State::CLIMB;
        }
    }
}

void Fred::stateRopeClimb(int input_x, int input_y)
{
    if (input_x == direction)
    {
        direction = -direction;
        frame = Frame::CLIMBING_CLAMPING;
        return;
    }
    else if (input_x == (-direction) &&
             sprite_pos.cy() == 0 &&
             !game.getGameMap().isStone(sprite_pos.cellPos(), input_x))
    {
        direction = input_x;
        startSideJump();
        return;
    }
    else if (input_y != 0)
    {
        if (sprite_pos.cy() == 0)
        {
            if (game.getGameMap().isStone(sprite_pos.cellPos(0, input_y)))
            {
                frame = Frame::CLIMBING_CLAMPING;
                return;
            }
        }
        if (frame == Frame::CLIMBING_CLAMPING)
            frame = Frame::CLIMBING_EXTENDING;
        else
            frame = Frame::CLIMBING_CLAMPING;
        game.addSound(climbing_sound);
        climbing_sound = climbing_sound == SoundID::CLIMBKOK ? SoundID::CLIMBTAK : SoundID::CLIMBKOK;
        sprite_pos.yadd(input_y);
        return;
    }
    else
    {
        frame = Frame::CLIMBING_CLAMPING;
    }
}

void Fred::stateExitMaze()
{
    if (sprite_pos.cy() == 0)
    {
        for (int i = static_cast<int>(SpriteClass::RAT);
             i <= static_cast<int>(SpriteClass::SMOKE); ++i)
            game.getSpriteList(static_cast<SpriteClass>(i)).clear();
    }
    collision_timer = 0;
    sprite_pos.yadd(-1);
    frame = frame == Frame::CLIMBING_CLAMPING ? Frame::CLIMBING_EXTENDING : Frame::CLIMBING_CLAMPING;
    if (sprite_pos.cy() == 0)
        game.setLevelStatus(GameBase::LevelStatus::NEXT_LEVEL);
}

void Fred::stateGameOver()
{
    color = static_cast<Color>(static_cast<int>(color) + 1);
}

void Fred::checkCollisionWithEnemy(Sprite const &other)
{
    // TODO: this function assumes that collisionInProgress has been checked before
    // calling it
    if (checkCollision(other))
    {
        if (game.decPower())
        {
            game.addSound(SoundID::COLLISION);
            collision_timer = 5;
            color = Color::RED;
        }
        else
        {
            game.setLevelStatus(GameBase::LevelStatus::GAME_OVER);
            state = State::GAME_OVER;
            color = Color::CYAN;
            collision_timer = 6;
        }
    }
}

void Fred::checkCollisionWithObject()
{
    auto &sprite_list = game.getSpriteList(SpriteClass::OBJECT);
    for (auto p = sprite_list.begin(), end = sprite_list.end(); p != end; ++p)
    {
        auto const &object_p = dynamic_cast<Object const *>(p->get());
        if (checkCollision(*object_p))
        {
            game.addSound(SoundID::PICK_OBJECT);
            object_p->apply(game);
            sprite_list.erase(p);
            break;
        }
    }
}

bool Fred::dbgResetPosition(CellPos const &cell_pos)
{
    if (state != State::WALK && state != State::CLIMB)
        return false;
    for (int dist = 0; dist < 20; ++dist)
    {
        for (int deltay = -dist; deltay <= (dist + 1); ++deltay)
        {
            int stepx = 1;
            if (deltay > -dist && deltay <= dist)
                stepx = 2 * dist + 1;
            for (int deltax = -dist; deltax <= (dist + 1); deltax += stepx)
            {
                auto block = game.getGameMap().getBlock(cell_pos, deltax, deltay);
                if (block == GameMap::Cell::EMPTY ||
                    block == GameMap::Cell::ROPE_END)
                {
                    sprite_pos = MapPos(cell_pos.x + deltax, cell_pos.y + deltay, 0, 1);
                    game.updateFredPos(sprite_pos, 1);
                    vposition = 1;
                    if (state != State::WALK)
                    {
                        state = State::WALK;
                        frame = Frame::STANDING;
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

bool Fred::dbgMoveToHatch()
{
    if (state != State::WALK && state != State::CLIMB)
        return false;
    auto hatch_pos = game.getGameMap().dbgGetHatchPos();
    sprite_pos = {hatch_pos.x, hatch_pos.y + 1, 0, 1};
    game.updateFredPos(sprite_pos, 1);
    vposition = 1;
    if (state != State::WALK)
    {
        state = State::WALK;
        frame = Frame::STANDING;
    }
    return true;
}

void Fred::dbgDie()
{
    game.setLevelStatus(GameBase::LevelStatus::GAME_OVER);
    state = State::GAME_OVER;
    color = Color::CYAN;
    collision_timer = 6;
}
