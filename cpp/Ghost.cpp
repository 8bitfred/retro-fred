#include "Ghost.hpp"
#include "GameMap.hpp"
#include "Game.hpp"

void Ghost::setRandomDirection()
{
    static Direction directions[] = {
        {-1,  0},
        { 1,  0},
        { 0, -1},
        { 0,  1},
    };
    std::uniform_int_distribution<> distrib(0, 3);
    while (true) {
        int choice = distrib(random_engine);
        if (direction != directions[choice]) {
            direction = directions[choice];
            break;
        }
    }
}

Ghost::Ghost(GameMap const &game_map, MapPos const &pos, std::minstd_rand &random_engine)
    : Sprite::Sprite(pos), game_map(game_map), random_engine(random_engine)
{
    setRandomDirection();
}

void Ghost::initialize(std::minstd_rand &random_engine, Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::GHOST);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < game.getSpriteCount().ghosts; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 1};
            if (game.getGameMap().isStone(pos.cellPos()))
                continue;
            sprite_list.emplace_back(std::make_unique<Ghost>(game.getGameMap(),
                                                             pos, random_engine));
            break;
        }
    }
}

void Ghost::update(unsigned)
{
    alternate_frame ^= 1;
    bool char_limit = false;
    bool map_limit = false;
    bool is_stone = false;
    if (direction.x > 0)
    {
        char_limit = sprite_pos.cx() == 1;
        map_limit = sprite_pos.x() == (game_map.getWidth() - 2);
        is_stone = game_map.isStone(sprite_pos.cellPos(), 1);
    }
    else if (direction.x < 0)
    {
        char_limit = sprite_pos.cx() == 0;
        map_limit = sprite_pos.x() == 1;
        is_stone = game_map.isStone(sprite_pos.cellPos(), -1);
    }
    else if (direction.y > 0)
    {
        char_limit = sprite_pos.cy() == 1;
        map_limit = sprite_pos.y() == (game_map.getHeight() - 2);
        is_stone = game_map.isStone(sprite_pos.cellPos(), 0, 1);
    }
    else
    {
        char_limit = sprite_pos.cy() == 1;
        map_limit = sprite_pos.y() == 1;
        is_stone = game_map.isStone(sprite_pos.cellPos(), 0, -1);
    }

    if (char_limit)
    {
        if (map_limit) 
        {
            setRandomDirection();
            return;
        }
        else if (is_stone)
        {
            std::uniform_int_distribution<> distrib(0, 7);
            if (distrib(random_engine) <= 1)
            {
                setRandomDirection();
                return;
            }
        }
    }
    sprite_pos.xadd(direction.x);
    sprite_pos.yadd(direction.y);
}

Sprite::BoxParams const &Ghost::getBoxParams() const
{
    static BoxParams box_params[2] = {
        {    9, 1, {-1, 0, 26, 33}, {{2, 3, 21, 29}} },
        { 50+9, 1, {-1, 0, 26, 33}, {{0, 3, 23, 29}} },
    };
    return box_params[alternate_frame];
}

Sprite::RenderParams Ghost::getRenderParams() const
{
    // Note that only one of direction.x and direction.y is not 0 at a given time
    bool hflip = (direction.x + direction.y) == 1;
    return {TextureID::GHOST, hflip, {}};
}

Label Ghost::getLabel() const
{
    bool hflip = (direction.x + direction.y) == 1;
    return labelOf(hflip ? LabelID::GHOST_RIGHT : LabelID::GHOST_LEFT);
}
