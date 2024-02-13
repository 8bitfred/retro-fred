#include "Rat.hpp"
#include "GameMap.hpp"
#include "Game.hpp"

void Rat::initialize(std::minstd_rand &random_engine, GameBase &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::RAT);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < game.getSpriteCount().rats; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 4};
            if (game.getGameMap().isStone(pos.cellPos()))
                continue;
            if (game.getGameMap().isStone(pos.cellPos(), -1))
                continue;
            if (game.getGameMap().isStone(pos.cellPos(), 1))
                continue;
            if (!game.getGameMap().isStone(pos.cellPos(), 0, 1))
                continue;
            if (!game.getGameMap().isStone(pos.cellPos(), -1, 1))
                continue;
            if (!game.getGameMap().isStone(pos.cellPos(), 1, 1))
                continue;
            sprite_list.emplace_back(std::make_unique<Rat>(game.getGameMap(), pos));
            break;
        }
    }
}

void Rat::update(unsigned)
{
    // cx_limit is 0 if we are moving left, or 2 if we are moving right
    int cx_limit = direction + 1;
    if (sprite_pos.cx() == cx_limit)
    {
        if (auto cell_pos = sprite_pos.cellPos();
            game_map.isStone(cell_pos, direction) || !game_map.isStone(cell_pos, direction, 1))
            direction = -direction;
        else
            sprite_pos.xadd(direction);
    }
    else
        sprite_pos.xadd(direction);
    alternate_frame = alternate_frame ^ 1;
}

Sprite::BoxParams const &Rat::getBoxParams() const
{
    static BoxParams box_params[2] = {
        {    1, 9, {0, -1, 16, 10}, {{2, 1, 12, 8}} },
        { 26+1, 9, {0, -1, 16, 10}, {{2, 1, 12, 8}} },
    };
    return box_params[alternate_frame];
}

Sprite::RenderParams Rat::getRenderParams() const
{
    return {TextureID::RAT, direction == 1, {}};
}

Label Rat::getLabel() const
{
    return labelOf(direction == -1 ? LabelID::RAT_LEFT : LabelID::RAT_RIGHT);
}
