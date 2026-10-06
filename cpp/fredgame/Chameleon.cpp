#include "Chameleon.hpp"
#include "fredcore/GameMap.hpp"
#include "fredcore/Game.hpp"

void Chameleon::initialize(std::minstd_rand &random_engine, GameBase &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::CHAMELEON);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    for (int i = 0; i < game.getSpriteCount().chameleons; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 0, 0};
            if (!Chameleon::isValidCell(game.getGameMap(), pos.cellPos()))
                continue;
            sprite_list.emplace_back(std::make_unique<Chameleon>(game.getGameMap(),
                                                                 pos, random_engine));
            break;
        }
    }
}

void Chameleon::update(unsigned)
{
    // State 2-Check X Position
    if (sprite_pos.cx() == 0 || sprite_pos.cx() == 3)
    {   // State 3-Set Wall Side
        std::uniform_int_distribution<> distrib(0, 63);
        if (distrib(random_engine) == 3)
        {
            direction.x = -direction.x;     // State 11-Toggle Horizontal Direction
            sprite_pos.xadd(direction.x);   // State 10-Move Horizontally
        }
        else if (int limit = direction.y < 0 ? 0 : 3;
                 sprite_pos.cy() == limit &&
                 !isValidCell(game_map, sprite_pos.cellPos(0, direction.y)))
            direction.y = -direction.y;     // State 12-Toggle Vertical Direction
        else
            sprite_pos.yadd(direction.y);   // State 8-Update Vert Coordinates
    }
    else
        sprite_pos.xadd(direction.x);       // State 10-Move Horizontally
    alternate_frame ^= 1;
}

bool Chameleon::isValidCell(GameMap const &game_map, CellPos const &pos)
{
    return !game_map.isStone(pos) &&
        game_map.isStone(pos, -1) &&
        game_map.isStone(pos, 1);
}

Sprite::BoxParams const &Chameleon::getBoxParams() const
{
    static BoxParams box_params[2][2][2] = {
        {
            {
                {      1, 1, {0, 0, 8, 16}, {} },
                {   18+1, 1, {0, 0, 8, 16}, {} },
            },
            {
                { 2*18+1, 1, {0, 0, 8, 16}, {} },
                { 3*18+1, 1, {0, 0, 8, 16}, {} },
            },
        },
        {
            {
                { 4*18+1, 1, {0, 0, 8, 16}, {} },
                { 5*18+1, 1, {0, 0, 8, 16}, {} },
            },
            {
                { 6*18+1, 1, {0, 0, 8, 16}, {} },
                { 7*18+1, 1, {0, 0, 8, 16}, {} },
            },
        },
    };
    // side is 0 if cx is 0 or 2, and 1 if cx is 1 or 3
    int side = sprite_pos.cx() % 2;
    // vdir is 0 if direction.y is -1 and 1 if direction.y is +1
    int vdir = (direction.y + 1) >> 1;
    return box_params[side][vdir][alternate_frame];
}

Sprite::RenderParams Chameleon::getRenderParams() const
{
    return {TextureID::CHAMELEON, false, {}};
}

Label Chameleon::getLabel() const
{
    // side is 0 if cx is 0 or 2, and 1 if cx is 1 or 3
    int side = sprite_pos.cx() % 2;
    // vdir is 0 if direction.y is -1 and 1 if direction.y is +1
    int vdir = (direction.y + 1) >> 1;
    auto index = static_cast<unsigned>(LabelID::CHAMELEON_LEFT_DOWN) + 2*side + vdir;
    return labelOf(static_cast<LabelID>(index));
}
