#include "Object.hpp"
#include "Game.hpp"

void Object::initialize(std::minstd_rand &random_engine, Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::OBJECT);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    std::uniform_int_distribution<> distrib_t(0, static_cast<int>(Object::Type::COUNT));
    // TODO: the number of objects depends on the game level. Also not all objects are
    // allowed in all levels.
    for (int counter = game.getSpriteCount().objects; counter > 0;)
    {
        MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 1, 3};
        if (game.getGameMap().getBlock(pos.cellPos()) != GameMap::Cell::EMPTY)
            continue;
        Object::Type object_type = {};
        if (counter >= 16)
            object_type = Object::Type::BULLETS;
        else if (counter <= static_cast<int>(Object::Type::COUNT))
            object_type = static_cast<Object::Type>(counter-1);
        else
            object_type = static_cast<Object::Type>(distrib_t(random_engine));
        while ((!game.getSpriteCount().has_busts && object_type == Object::Type::BUST) ||
               (!game.getSpriteCount().has_stones && object_type == Object::Type::STONE) ||
               (!game.getSpriteCount().has_masks && object_type == Object::Type::MASK))
        {
            object_type = static_cast<Object::Type>(distrib_t(random_engine));
        }
        sprite_list.emplace_back(std::make_unique<Object>(pos, object_type));
        --counter;
    }
}

void Object::apply(Game &game) const
{
    switch (type)
    {
        case Type::MAP:
            game.setMinimapPos(game.getFredPos().cellPos());
            break;
        case Type::LIFE:
            game.incPower();
            break;
        case Type::BUST:
            game.addTreasure(800);
            break;
        case Type::STONE:
            game.addTreasure(1000);
            break;
        case Type::BULLETS:
            game.rechargeBullets();
            break;
        case Type::STATUE:
            game.addTreasure(500);
            break;
        case Type::MASK:
            game.addTreasure(1500);
            break;
        case Type::AMULET:
            game.addTreasure(500);
            break;
        default:;
    }
}

Sprite::BoxParams const &Object::getBoxParams() const
{
    static BoxParams box_params[] = {
        {       9,      9, { -1,  -1, 18, 18}, {{1, 1, 15, 14}} },
        {   42+10,      9, { -1,  -1, 18, 18}, {{3, 0, 11, 16}} },
        { 2*42+10,      9, { -1,  -1, 18, 18}, {{0, 0, 13, 16}} },
        { 3*42+10,      9, { -1,  -1, 18, 18}, {{0, 0, 15, 15}} },
        { 4*42+10,      9, { -1,  -1, 18, 18}, {{5, 0,  7, 16}} },
        { 5*42+10,      9, { -1,  -1, 18, 18}, {{2, 2, 10, 14}} },
        { 6*42+10,      9, { -1,  -1, 18, 18}, {{0, 1, 16, 15}} },
        { 7*42+10,     10, { -1,  -1, 18, 18}, {{2, 0, 11, 16}} },
    };
    static_assert(std::size(box_params) == static_cast<int>(Type::COUNT));
    return box_params[static_cast<int>(type)];
}

Sprite::RenderParams Object::getRenderParams() const
{
    return {TextureID::OBJECT, false, {}};
}
