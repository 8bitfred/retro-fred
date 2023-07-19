#include "Fred.hpp"
#include "Game.hpp"
#include "GameMap.hpp"
#include <functional>

Fred::Fred(Frame const &frame, MapPos initial_position)
: Sprite(frame, initial_position, 4, 4)
{

}

Sprite::RenderInfo Fred::getTexture() const
{
    // The FRED_SHEET texture contains the sprite sheet for Fred. The sheet is organized
    // as a 4x4 matrix, where each cell in the matrix is a different frame. Each cell is
    // 50 by 50 pixels, and each cell is separated from each other by 8 pixels. Each cell
    // has a 1 pixel rectangle surrounding the frame, so each frame is actually 48x48
    // pixels.

    // Offset of cells 0, 1, 2 and 3:
    static constexpr int c0 = 1;
    static constexpr int c1 = c0 + 58;
    static constexpr int c2 = c1 + 58;
    static constexpr int c3 = c2 + 58;
    static RenderInfo textures[2][7] =
        {
            {
            /* STANDING      ( 1) */ {TextureID::FRED, {c0, c0, 48, 48}, 8, 8},
            /* BIG_STEP      ( 2) */ {TextureID::FRED, {c1, c0, 48, 48}, 8, 8},
            /* SMALL_STEP    ( 3) */ {TextureID::FRED, {c2, c0, 48, 48}, 8, 8},
            /* CLIMBING1     ( 7) */ {TextureID::FRED, {c2, c1, 48, 48}, 8, 8},
            /* CLIMBING2     ( 8) */ {TextureID::FRED, {c3, c1, 48, 48}, 8, 8},
            /* SHOOTING      (11) */ {TextureID::FRED, {c2, c2, 48, 48}, 8, 8},
            /* JUMP_SHOOTING (13) */ {TextureID::FRED, {c0, c3, 48, 48}, 8, 8},
            },
            {
            /* STANDING      ( 4) */ {TextureID::FRED, {c3, c0, 48, 48}, 8, 8},
            /* BIG_STEP      ( 5) */ {TextureID::FRED, {c0, c1, 48, 48}, 8, 8},
            /* SMALL_STEP    ( 6) */ {TextureID::FRED, {c1, c1, 48, 48}, 8, 8},
            /* CLIMBING1     ( 9) */ {TextureID::FRED, {c0, c2, 48, 48}, 8, 8},
            /* CLIMBING2     (10) */ {TextureID::FRED, {c1, c2, 48, 48}, 8, 8},
            /* SHOOTING      (12) */ {TextureID::FRED, {c3, c2, 48, 48}, 8, 8},
            /* JUMP_SHOOTING (14) */ {TextureID::FRED, {c1, c3, 48, 48}, 8, 8},
            },
        };
    int dir_index = (frame_dir + 1) >> 1;
    return textures[dir_index][static_cast<int>(frame_type)];
}

void Fred::updateFred(Game& game, unsigned events)
{
    int hmove = 0, vmove = 0;
    bool fire = false;
    if ((events & Game::EVENT_LEFT) != 0)
        hmove = -1;
    else if ((events & Game::EVENT_RIGHT) != 0)
        hmove = 1;
    else if ((events & Game::EVENT_UP) != 0)
        vmove = -1;
    else if ((events & Game::EVENT_DOWN) != 0)
        vmove = 1;
    if ((events & Game::EVENT_FIRE) != 0)
        fire = true;

    switch (state)
    {
    case State::WALK:
        stateWalk(game, hmove, vmove, fire);
        break;
    case State::VERTICAL_JUMP:
        stateVerticalJump(game, hmove, vmove, fire);
        break;
    case State::SIDE_JUMP:
        stateSideJump(game, hmove, vmove, fire);
        break;
    case State::ROPE_CLIMB:
        stateRopeClimb(game, hmove, vmove, fire);
        break;
    case State::EXIT_MAZE:
        break;
    default:
        break;
    }
}

void Fred::stateWalk(Game& game, int hmove, int vmove, bool fire)
{
    if (hmove == (-frame_dir)) 
    {
        frame_dir = -frame_dir;
        frame_type = FrameType::STANDING;
    }
    else if (hmove == frame_dir)
        walkOneStep(game, fire);
    else if (vmove < 0)
        startVerticalJump(game, fire);
    else if (fire)
    {
        frame_type = FrameType::SHOOTING;
    }
    else 
        frame_type = FrameType::STANDING;
}

void Fred::walkOneStep(Game &game, bool fire)
{
    if (sprite_pos.cx == 0)
    {
        if (game.getGameMap().isStone(nextCellPos()))
        {
            if (frame_type != FrameType::STANDING)
                game.playSound(SoundID::WALK);
            frame_type = FrameType::STANDING;
            return;
        }
        else if (auto next_cell = game.getGameMap().getCell(nextCellPos().vmove(1));
                 next_cell == GameMap::Cell::ROPE_MAIN)
        {
            sprite_pos.yadd(-1);
            startSideJump(game, fire);
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
    {
        frame_type = FrameType::STANDING;
        game.playSound(SoundID::WALK);
    }
    sprite_pos.xadd(frame_dir);
    game.moveFrame(frame_dir, 0);
}

void Fred::startSideJump(Game& game, bool fire)
{
    if (fire)
        frame_type = FrameType::JUMP_SHOOTING;
    else
        frame_type = FrameType::BIG_STEP;
    state = State::SIDE_JUMP;
    jump_stage = 3;
    sprite_pos.xadd(frame_dir);
    game.moveFrame(frame_dir, 0);
    game.playSound(SoundID::JUMP);
}

void Fred::startVerticalJump(Game& game, bool fire)
{
    if (fire)
        frame_type = FrameType::JUMP_SHOOTING;
    else
        frame_type = FrameType::BIG_STEP;
    state = State::VERTICAL_JUMP;
    jump_stage = 3;
    sprite_pos.yadd(-1);
    game.playSound(SoundID::JUMP);
}

void Fred::stateVerticalJump(Game& game, int, int, bool fire)
{
    --jump_stage;
    if (jump_stage == 1 &&
        sprite_pos.cx == 0 &&
        game.getGameMap().getCell(sprite_pos.cellPos()) == GameMap::Cell::ROPE_END)
    {
        frame_type = FrameType::CLIMBING1;
        state = State::ROPE_CLIMB;
    }
    else if (jump_stage == 0)
    {
        sprite_pos.yadd(1);
        if (fire)
        {
            frame_type = FrameType::SHOOTING;
        }
        else
        {
            frame_type = FrameType::STANDING;
            game.playSound(SoundID::WALK);
        }
        state = State::WALK;
    }
    else if (fire)
    {
        frame_type = FrameType::JUMP_SHOOTING;
    }
    else
        frame_type = FrameType::BIG_STEP;
}

void Fred::stateSideJump(Game& game, int, int, bool fire)
{
    --jump_stage;
    sprite_pos.xadd(frame_dir);
    game.moveFrame(frame_dir, 0);
    if (jump_stage == 0)
    {
        if (game.getGameMap().isStone(sprite_pos.cellPos().vmove(1)))
        {
            sprite_pos.yadd(1);
            if (fire)
                frame_type = FrameType::SHOOTING;
            else
            {
                frame_type = FrameType::STANDING;
                game.playSound(SoundID::WALK);
            }
            state = State::WALK;
        }
        else 
        {
            frame_type = FrameType::CLIMBING1;
            state = State::ROPE_CLIMB;
        }
    }
    else if (fire)
        frame_type = FrameType::JUMP_SHOOTING;
    else
        frame_type = FrameType::BIG_STEP;
}

void Fred::stateRopeClimb(Game& game, int hmove, int vmove, bool fire)
{
    if (hmove == frame_dir)
    {
        frame_dir = -frame_dir;
        frame_type = FrameType::CLIMBING1;
        return;
    }
    else if (hmove == (-frame_dir) &&
             sprite_pos.cy == 0 &&
             !game.getGameMap().isStone(sprite_pos.cellPos().hmove(hmove)))
    {
        frame_dir = hmove;
        startSideJump(game, fire);
        return;
    }
    else if (vmove != 0)
    {
        if (sprite_pos.cy == 0)
        {
            auto next_pos = sprite_pos.cellPos().vmove(vmove);
            if (game.getGameMap().isStone(next_pos))
            {
                frame_type = FrameType::CLIMBING1;
                return;
            }
            else if (game.getGameMap().getCell(next_pos) == GameMap::Cell::TRAPDOOR)
            {
                // TODO: maze exit;
                return;
            }
        }
        if (frame_type == FrameType::CLIMBING1)
        {
            frame_type = FrameType::CLIMBING2;
            game.playSound(SoundID::CLIMB2);
        }
        else
        {
            frame_type = FrameType::CLIMBING1;
            game.playSound(SoundID::CLIMB1);
        }
        sprite_pos.yadd(vmove);
        state = State::ROPE_CLIMB;
        game.moveFrame(0, vmove);
        return;
    }
    else
    {
        frame_type = FrameType::CLIMBING1;
    }
}

void Fred::dbgResetPosition(Game &game)
{
    if (state != State::WALK)
        return;
    MapPos candidate{game.getFrame().gFrame().x + game.getFrame().getFredOffsetX(),
                     game.getFrame().gFrame().y + game.getFrame().getFredOffsetY(),
                     0, 1};
    auto max_x = game.getGameMap().getWidth() - 2;
    auto max_y = game.getGameMap().getHeight() - 2;
    candidate.x = std::max(1, std::min(max_x, candidate.x));
    candidate.y = std::max(1, std::min(max_y, candidate.y));
    candidate.y += (candidate.y % 2) - 1;

    for (int deltax = 0; true; ++deltax)
    {
        if (auto cell_pos = candidate.cellPos().hmove(deltax);
            cell_pos.x <= max_x && 
            game.getGameMap().getCell(cell_pos) == GameMap::Cell::EMPTY)
        {
            candidate.x = cell_pos.x;
            break;
        }
        if (auto cell_pos = candidate.cellPos().hmove(-deltax);
            deltax > 0 && cell_pos.x >= 1 && 
            game.getGameMap().getCell(cell_pos) == GameMap::Cell::EMPTY)
        {
            candidate.x = cell_pos.x;
            break;
        }
    }

    sprite_pos = candidate;
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
