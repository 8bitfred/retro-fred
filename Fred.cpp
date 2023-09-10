#include "Fred.hpp"
#include "Game.hpp"
#include "GameMap.hpp"
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
        { 3*58+9,      9, {-1, -1, 34, 34},       // CLIMBING1
            {{14, 1, 18, 23}, {10, 20,  9, 11}} },
        {      9,   58+9, {-1, -1, 34, 34},       // CLIMBING2
            {{13, 0, 19, 23}, {10, 21, 11, 11}} },
        {   58+9,   58+9, {-8, -1, 48, 34},       // SHOOTING_STANDING
            {{ 9, 0, 14, 32}, { 0,  9, 26, 14}} },
        { 2*58+9,   58+9, {-8, -1, 48, 34},       // SHOOTING_BIG_STEP
            {{ 9, 1, 14, 31}, { 0, 10, 32, 22}} },
        { 3*58+9,   58+9, {-8, -1, 48, 34},       // SHOOTING_SMALL_STEP
            {{ 9, 1, 14, 31}, { 0, 10, 24, 13}, { 7, 24, 19, 8}} },
    };

    int frame_index = static_cast<int>(frame);
    if (shooting && frame_index <= static_cast<int>(Frame::SMALL_STEP))
        frame_index += static_cast<int>(Frame::SHOOTING_STANDING);
    return box_params[frame_index];
}

Sprite::RenderParams Fred::getRenderParams() const
{
    ColorModulation color_mod;
    if (collision_timer != 0)
        color_mod = {255, 0, 0};
    else
        color_mod = {255, 255, 0};
    return {TextureID::FRED, direction == 1, color_mod};
}

void Fred::updateFred(Game& game, unsigned events)
{
    int input_x = 0, input_y = 0;
    bool input_fire = false;
    if ((events & Game::EVENT_UP) != 0)
        input_y = -1;
    else if ((events & Game::EVENT_DOWN) != 0)
        input_y = 1;
    else if ((events & Game::EVENT_LEFT) != 0)
        input_x = -1;
    else if ((events & Game::EVENT_RIGHT) != 0)
        input_x = 1;
    if ((events & Game::EVENT_FIRE) != 0)
        input_fire = true;

    switch (state)
    {
    case State::WALK:
        stateWalk(game, input_x, input_y);
        break;
    case State::VERTICAL_JUMP:
        stateVerticalJump(game, input_x, input_y);
        break;
    case State::SIDE_JUMP:
        stateSideJump(game, input_x, input_y);
        break;
    case State::CLIMB:
        stateRopeClimb(game, input_x, input_y);
        break;
    case State::EXIT_MAZE:
        break;
    default:
        break;
    }
    checkFire(game, input_fire);
    if (collision_timer > 0)
        --collision_timer;
}

void Fred::checkFire(Game &game, bool input_fire)
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

void Fred::stateWalk(Game& game, int input_x, int input_y)
{
    if (input_x == (-direction)) 
    {
        direction = -direction;
        frame = Frame::STANDING;
    }
    else if (input_x == direction)
        walkOneStep(game);
    else if (input_y < 0)
        startVerticalJump(game);
    else 
        frame = Frame::STANDING;
}

void Fred::walkOneStep(Game &game)
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
            startSideJump(game);
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
    game.moveFrame(direction, 0);
}

void Fred::startSideJump(Game& game)
{
    frame = Frame::BIG_STEP;
    state = State::SIDE_JUMP;
    jump_stage = 3;
    sprite_pos.xadd(direction);
    game.moveFrame(direction, 0);
    game.addSound(SoundID::JUMP);
}

void Fred::startVerticalJump(Game& game)
{
    frame = Frame::BIG_STEP;
    state = State::VERTICAL_JUMP;
    jump_stage = 3;
    sprite_pos.yadd(-1);
    game.addSound(SoundID::JUMP);
}

void Fred::stateVerticalJump(Game& game, int, int)
{
    --jump_stage;
    if (jump_stage == 1 &&
        sprite_pos.cx() == 0 &&
        game.getGameMap().getBlock(sprite_pos.cellPos()) == GameMap::Cell::ROPE_END)
    {
        frame = Frame::CLIMBING1;
        state = State::CLIMB;
    }
    else if (jump_stage == 0)
    {
        sprite_pos.yadd(1);
        frame = Frame::STANDING;
        game.addSound(SoundID::WALK);
        state = State::WALK;
    }
}

void Fred::stateSideJump(Game& game, int, int)
{
    --jump_stage;
    sprite_pos.xadd(direction);
    game.moveFrame(direction, 0);
    if (jump_stage == 0)
    {
        if (game.getGameMap().isStone(sprite_pos.cellPos(), 0, 1))
        {
            sprite_pos.yadd(1);
            frame = Frame::STANDING;
            game.addSound(SoundID::WALK);
            state = State::WALK;
        }
        else 
        {
            frame = Frame::CLIMBING1;
            state = State::CLIMB;
        }
    }
}

void Fred::stateRopeClimb(Game& game, int input_x, int input_y)
{
    if (input_x == direction)
    {
        direction = -direction;
        frame = Frame::CLIMBING1;
        return;
    }
    else if (input_x == (-direction) &&
             sprite_pos.cy() == 0 &&
             !game.getGameMap().isStone(sprite_pos.cellPos(), input_x))
    {
        direction = input_x;
        startSideJump(game);
        return;
    }
    else if (input_y != 0)
    {
        if (sprite_pos.cy() == 0)
        {
            auto next_pos = sprite_pos.cellPos(0, input_y);
            if (game.getGameMap().isStone(next_pos))
            {
                frame = Frame::CLIMBING1;
                return;
            }
            else if (game.getGameMap().getBlock(next_pos) == GameMap::Cell::TRAPDOOR)
            {
                // TODO: maze exit;
                return;
            }
        }
        if (frame == Frame::CLIMBING1)
            frame = Frame::CLIMBING2;
        else
            frame = Frame::CLIMBING1;
        game.addSound(climbing_sound);
        climbing_sound = climbing_sound == SoundID::CLIMB1 ? SoundID::CLIMB2 : SoundID::CLIMB1;
        sprite_pos.yadd(input_y);
        game.moveFrame(0, input_y);
        return;
    }
    else
    {
        frame = Frame::CLIMBING1;
    }
}

void Fred::checkCollisionWithEnemy(Game &game, Sprite const &other)
{
    if (checkCollision(other))
    {
        collision_timer = 5;
        game.addSound(SoundID::COLLISION);
    }
}

void Fred::dbgResetPosition(Game &game)
{
    if (state != State::WALK)
        return;
    auto candidate = game.getFrame().gFrame().cellPos(game.getFrame().getFredOffsetX(),
                                                      game.getFrame().getFredOffsetY());
    auto max_x = game.getGameMap().getWidth() - 2;
    auto max_y = game.getGameMap().getHeight() - 2;
    candidate.x = std::max(1, std::min(max_x, candidate.x));
    candidate.y = std::max(1, std::min(max_y, candidate.y));
    candidate.y += (candidate.y % 2) - 1;

    for (int deltax = 0; true; ++deltax)
    {
        if (CellPos cell_pos{candidate.x + deltax, candidate.y};
            cell_pos.x <= max_x && 
            game.getGameMap().getBlock(cell_pos) == GameMap::Cell::EMPTY)
        {
            sprite_pos = MapPos(cell_pos.x, cell_pos.y, 0, 1);
            break;
        }
        if (CellPos cell_pos{candidate.x - deltax, candidate.y};
            deltax > 0 && cell_pos.x >= 1 && 
            game.getGameMap().getBlock(cell_pos) == GameMap::Cell::EMPTY)
        {
            sprite_pos = MapPos(cell_pos.x, cell_pos.y, 0, 1);
            break;
        }
    }

    game.getFrame().adjustFramePos(sprite_pos);
    game.dbgResetMapBlocks();
}

void Fred::dbgMoveToHatch(Game &game)
{
    if (state != State::WALK)
        return;
    auto hatch_pos = game.getGameMap().dbgGetHatchPos();
    sprite_pos = {hatch_pos.x, hatch_pos.y + 1, 0, 1};
    game.getFrame().adjustFramePos(sprite_pos);
    game.dbgResetMapBlocks();
}
