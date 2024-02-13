#include "Mummy.hpp"
#include "GameMap.hpp"
#include "Game.hpp"

int Mummy::mummy_timer = 0;

Mummy::Mummy(GameMap const &game_map, std::minstd_rand &random_engine)
    : Sprite::Sprite(getRandomLocation(random_engine, game_map))
    , game_map(game_map)
    , random_engine(random_engine)
    , direction(getRandomDirection(random_engine))
{
}

void Mummy::initialize(std::minstd_rand &random_engine, GameBase &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::MUMMY);
    for (int i = 0; i < game.getSpriteCount().mummies; ++i) {
        sprite_list.emplace_back(std::make_unique<Mummy>(game.getGameMap(), random_engine));
    }
}

void Mummy::update(unsigned)
{
    if (state == State::WALK)
        stateWalk();
    else if (state == State::FALL)
        stateFall();
    else if (state == State::BOUNCE)
        stateBounce();
    else if (state == State::DISAPPEAR)
        stateDisappear();
}

void Mummy::stateWalk()
{
    // 0 if direction < 0, 1 if direction > 0
    int stone_limit = (direction + 1) / 2;
    // 2 if direction < 0, 3 if direction > 0
    int rope_limit = stone_limit + 2;
    // 0 if direction < 0, 1 if direction > 0
    int x_below = stone_limit;
    if (sprite_pos.cx() == stone_limit)
    {
        if (game_map.isStone(sprite_pos.cellPos(), direction))
        {
            if (flip)
            {
                frame = Frame::POP;
                state = State::DISAPPEAR;
            }
            else
            {
                flip = true;
                direction = -direction;
                frame = Frame::STANDING;
            }
            return;
        }
    }
    else if (sprite_pos.cx() == rope_limit)
    {
        if (!game_map.isStone(sprite_pos.cellPos(), x_below, 1))
        {
            frame = Frame::FALL;
            state = State::FALL;
            sprite_pos.xadd(direction);
            sprite_pos.yadd(2);
            return;
        }
    }
    if (mummy_timer == 1)
    {
        sprite_pos.xadd(direction);
        frame = frame == Frame::STANDING ? Frame::STEP : Frame::STANDING;
    }
}

void Mummy::stateFall()
{
    if (sprite_pos.cy() == 1 &&
        game_map.isStone(sprite_pos.cellPos(), 0, 1))
    {
        state = State::BOUNCE;
        return;
    }
    sprite_pos.yadd(2);
}

void Mummy::stateBounce()
{
    if (sprite_pos.cy() == 1)
        sprite_pos.yadd(-1);
    else 
    {
        sprite_pos.yadd(1);
        state = State::WALK;
        frame = Frame::STANDING;
        flip = false;
    }
}

void Mummy::stateDisappear()
{
    sprite_pos = getRandomLocation(random_engine, game_map);
    direction = getRandomDirection(random_engine);
    state = State::WALK;
    frame = Frame::STANDING;
    flip = false;
}

Sprite::BoxParams const &Mummy::getBoxParams() const
{
    static BoxParams box_params[] = {
        {      1, 9, {0, -1, 24, 34}, {{2, 0, 20, 32}} },
        {   34+1, 9, {0, -1, 24, 34}, {{2, 0, 20, 32}} },
        { 2*34+1, 9, {0, -1, 24, 34}, {{1, 5, 19, 23}} },
        { 3*34+1, 9, {0, -1, 24, 34}, {{1, 5, 19, 23}} },
    };
    static_assert(static_cast<int>(Frame::COUNT) == std::size(box_params));
    return box_params[static_cast<int>(frame)];
}

Sprite::RenderParams Mummy::getRenderParams() const
{
    return {TextureID::MUMMY, direction == 1 && frame != Frame::POP, {}};
}

Label Mummy::getLabel() const
{
    unsigned index;
    unsigned dir_index = (direction + 1) / 2;
    if (frame == Frame::STANDING || frame == Frame::STEP)
        index = static_cast<unsigned>(LabelID::MUMMY_LEFT) + dir_index;
    else if (frame == Frame::FALL)
        index = static_cast<unsigned>(LabelID::MUMMY_FALLING_LEFT) + dir_index;
    else
        index = static_cast<unsigned>(LabelID::MUMMY_POP);
    return labelOf(static_cast<LabelID>(index));
}

MapPos Mummy::getRandomLocation(std::minstd_rand &random_engine,
                                GameMap const &game_map)
{
    std::uniform_int_distribution<> distrib_x(1, game_map.getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, (game_map.getHeight() / 2) - 1);
    while (true)
    {
        MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 1};
        if (game_map.getBlock(pos.cellPos()) == GameMap::Cell::EMPTY)
            return pos;
    }
}

int Mummy::getRandomDirection(std::minstd_rand &random_engine)
{
    std::uniform_int_distribution<> distrib(0, 1);
    return 2 * distrib(random_engine) - 1;
}
