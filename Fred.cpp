#include "Fred.hpp"
#include "Game.hpp"
#include "GameMap.hpp"
#include <functional>

Fred::Fred(Window const &window, MapPos initial_position)
: Sprite(window, initial_position, 4, 4)
{

}

Sprite::RenderInfo const &Fred::getTexture() const
{
    // The FRED_SHEET texture contains the sprite sheet for Fred. The sheet is organized
    // as a 4x4 matrix, where each cell in the matrix is a different window. Each cell is
    // 50 by 50 pixels, and each cell is separated from each other by 8 pixels. Each cell
    // has a 1 pixel rectangle surrounding the window, so each window is actually 48x48
    // pixels.

    // Offset of cells 0, 1, 2 and 3:
    static constexpr int c0 = 1;
    static constexpr int c1 = c0 + 58;
    static constexpr int c2 = c1 + 58;
    static constexpr int c3 = c2 + 58;
    static RenderInfo textures[2][8] =
        {
            {
            /* STANDING            */ {TextureID::FRED, {c0, c0, 48, 48}, 8, 8},
            /* BIG_STEP            */ {TextureID::FRED, {c1, c0, 48, 48}, 8, 8},
            /* SMALL_STEP          */ {TextureID::FRED, {c2, c0, 48, 48}, 8, 8},
            /* CLIMBING1           */ {TextureID::FRED, {c2, c1, 48, 48}, 8, 8},
            /* CLIMBING2           */ {TextureID::FRED, {c3, c1, 48, 48}, 8, 8},
            /* SHOOTING_STANDING   */ {TextureID::FRED, {c2, c2, 48, 48}, 8, 8},
            /* SHOOTING_BIG_STEP   */ {TextureID::FRED, {c3, c2, 48, 48}, 8, 8},
            /* SHOOTING_SMALL_STEP */ {TextureID::FRED, {c0, c3, 48, 48}, 8, 8},
            },
            {
            /* STANDING            */ {TextureID::FRED, {c3, c0, 48, 48}, 8, 8},
            /* BIG_STEP            */ {TextureID::FRED, {c0, c1, 48, 48}, 8, 8},
            /* SMALL_STEP          */ {TextureID::FRED, {c1, c1, 48, 48}, 8, 8},
            /* CLIMBING1           */ {TextureID::FRED, {c0, c2, 48, 48}, 8, 8},
            /* CLIMBING2           */ {TextureID::FRED, {c1, c2, 48, 48}, 8, 8},
            /* SHOOTING_STANDING   */ {TextureID::FRED, {c1, c3, 48, 48}, 8, 8},
            /* SHOOTING_BIG_STEP   */ {TextureID::FRED, {c2, c3, 48, 48}, 8, 8},
            /* SHOOTING_SMALL_STEP */ {TextureID::FRED, {c3, c3, 48, 48}, 8, 8},
            },
        };
    int dir_index = (direction + 1) >> 1;
    int frame_index = static_cast<int>(frame);
    if (shooting && frame_index <= static_cast<int>(Frame::SMALL_STEP))
        frame_index += static_cast<int>(Frame::SHOOTING_STANDING);
    // TODO: we should not be modifying the static structure.
    auto &render_info = textures[dir_index][frame_index];
    render_info.texture_id = collisionInProgress() ? TextureID::FRED_RED : TextureID::FRED;
    return render_info;
}

std::vector<SDL_Rect> const &Fred::getHitBoxes() const
{
    static std::vector<SDL_Rect> boxes[2][8] =
        {
            {
            /* STANDING            */ {{ 9, 0, 14, 32}, { 0, 13, 26, 10}},
            /* BIG_STEP            */ {{ 9, 1, 14, 31}, { 0, 12, 32, 20}},
            /* SMALL_STEP          */ {{ 9, 1, 14, 31}, { 0, 15, 24,  8}, { 7, 24, 19, 8}},
            /* CLIMBING1           */ {{14, 1, 18, 23}, {10, 20,  9, 10}},
            /* CLIMBING2           */ {{13, 0, 19, 23}, {10, 21, 11, 10}},
            /* SHOOTING_STANDING   */ {{ 9, 0, 14, 32}, { 0,  9, 26, 14}},
            /* SHOOTING_BIG_STEP   */ {{ 9, 1, 14, 31}, { 0, 10, 32, 22}},
            /* SHOOTING_SMALL_STEP */ {{ 9, 1, 14, 31}, { 0, 10, 24, 13}, { 7, 24, 19, 8}},
            },
            {
            /* STANDING            */ {{ 9, 0, 14, 32}, { 6, 13, 26, 10}},
            /* BIG_STEP            */ {{ 9, 1, 14, 31}, { 0, 12, 32, 20}},
            /* SMALL_STEP          */ {{ 9, 1, 14, 31}, { 8, 15, 24,  8}, { 6, 24, 19, 8}},
            /* CLIMBING1           */ {{ 0, 1, 18, 23}, {13, 20,  9, 10}},
            /* CLIMBING2           */ {{ 0, 0, 19, 23}, {11, 21, 11, 10}},
            /* SHOOTING_STANDING   */ {{ 9, 0, 14, 32}, { 6,  9, 26, 14}},
            /* SHOOTING_BIG_STEP   */ {{ 9, 1, 14, 31}, { 0, 10, 32, 22}},
            /* SHOOTING_SMALL_STEP */ {{ 9, 1, 14, 31}, { 8, 10, 24, 13}, { 6, 24, 19, 8}},
            },
        };
    int dir_index = (direction + 1) >> 1;
    int frame_index = static_cast<int>(frame);
    if (shooting && frame_index <= static_cast<int>(Frame::SMALL_STEP))
        frame_index += static_cast<int>(Frame::SHOOTING_STANDING);
    return boxes[dir_index][frame_index];
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
        game.playSound(SoundID::WALK);
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
    game.playSound(SoundID::JUMP);
}

void Fred::startVerticalJump(Game& game)
{
    frame = Frame::BIG_STEP;
    state = State::VERTICAL_JUMP;
    jump_stage = 3;
    sprite_pos.yadd(-1);
    game.playSound(SoundID::JUMP);
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
        game.playSound(SoundID::WALK);
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
            game.playSound(SoundID::WALK);
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
        game.playSound(climbing_sound);
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

void Fred::checkCollisionWithEnemy(Sprite const &other)
{
    if (checkCollision(other))
        collision_timer = 5;
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
