#include "Mummy.hpp"
#include "Game.hpp"

Mummy::Mummy(Game& game, std::minstd_rand &random_engine)
    : Sprite::Sprite(game.getFrame(), getRandomLocation(random_engine, game.getGameMap()), 
    3, 4)
    , random_engine(random_engine)
    , frame_dir(getRandomDirection(random_engine))
{
}

void Mummy::update(Game &game, unsigned)
{
    if (state == State::WALK)
        stateWalk(game);
    else if (state == State::FALL)
        stateFall(game);
    else if (state == State::BOUNCE)
        stateBounce(game);
    else if (state == State::DISAPPEAR)
        stateDisappear(game);
}

void Mummy::stateWalk(Game &game)
{
    // 0 if frame_dir < 0, 1 if frame_dir > 0
    int stone_limit = (frame_dir + 1) / 2;
    int rope_limit = stone_limit + 2;
    int x_below = stone_limit;
    if (sprite_pos.cx == stone_limit)
    {
        if (game.getGameMap().isStone(sprite_pos.cellPos().hmove(frame_dir)))
        {
            if (flip)
            {
                frame_type = FrameType::POP;
                state = State::DISAPPEAR;
            }
            else
            {
                flip = true;
                frame_dir = -frame_dir;
            }
            return;
        }
    }
    else if (sprite_pos.cx == rope_limit)
    {
        auto cell_below = sprite_pos.cellPos().hmove(x_below).vmove(1);
        if (!game.getGameMap().isStone(cell_below))
        {
            frame_type = FrameType::FALL;
            state = State::FALL;
            sprite_pos.xadd(frame_dir);
            sprite_pos.yadd(2);
            return;
        }
    }
    mummy_timer ^= 1;
    if (mummy_timer == 0)
    {
        sprite_pos.xadd(frame_dir);
        frame_type = frame_type == FrameType::STANDING ? FrameType::STEP : FrameType::STANDING;
    }
}

void Mummy::stateFall(Game &game)
{
    if (sprite_pos.cy == 1 &&
        game.getGameMap().isStone(sprite_pos.cellPos().vmove(1)))
    {
        sprite_pos.yadd(-1);
        state = State::BOUNCE;
        return;
    }
    sprite_pos.yadd(2);
}

void Mummy::stateBounce(Game &)
{
    sprite_pos.yadd(1);
    state = State::WALK;
    frame_type = FrameType::STANDING;
    flip = false;
}

void Mummy::stateDisappear(Game &game)
{
    sprite_pos = getRandomLocation(random_engine, game.getGameMap());
    frame_dir = getRandomDirection(random_engine);
    state = State::WALK;
    frame_type = FrameType::STANDING;
    flip = false;
}

Sprite::RenderInfo Mummy::getTexture() const
{
    static constexpr int c0 = 1;
    static constexpr int c1 = c0 + 34;
    static constexpr int c2 = c1 + 34;
    static constexpr int c3 = c2 + 34;
    static constexpr int c4 = c3 + 34;
    static constexpr int c5 = c4 + 34;
    static constexpr int c6 = c5 + 34;

    static RenderInfo textures[2][4] =
        {
            {
                {TextureID::MUMMY, {c0, 8, 24, 34}, 0, 1},
                {TextureID::MUMMY, {c1, 8, 24, 34}, 0, 1},
                {TextureID::MUMMY, {c2, 8, 24, 34}, 0, 1},
                {TextureID::MUMMY, {c6, 8, 24, 34}, 0, 1},
            },
            {
                {TextureID::MUMMY, {c3, 8, 24, 34}, 0, 1},
                {TextureID::MUMMY, {c4, 8, 24, 34}, 0, 1},
                {TextureID::MUMMY, {c5, 8, 24, 34}, 0, 1},
                {TextureID::MUMMY, {c6, 8, 24, 34}, 0, 1},
            },
        };
    int dir_index = (frame_dir + 1) >> 1;
    return textures[dir_index][static_cast<int>(frame_type)];
}

MapPos Mummy::getRandomLocation(std::minstd_rand &random_engine,
                                GameMap const &game_map)
{
    std::uniform_int_distribution<> distrib_x(1, game_map.getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, (game_map.getHeight() / 2) - 1);
    while (true)
    {
        MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 1};
        if (game_map.getCell(pos.cellPos()) == GameMap::Cell::EMPTY)
            return pos;
    }
}

int Mummy::getRandomDirection(std::minstd_rand &random_engine)
{
    std::uniform_int_distribution<> distrib(0, 1);
    return 2 * distrib(random_engine) - 1;
}
