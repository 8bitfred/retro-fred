#include "AcidDrop.hpp"
#include "Game.hpp"

void AcidDrop::initialize(std::minstd_rand &random_engine, Game &game)
{
    auto &sprite_list = game.getSpriteList(SpriteClass::ACID_DROP);
    std::uniform_int_distribution<> distrib_x(1, game.getGameMap().getWidth() - 2);
    std::uniform_int_distribution<> distrib_y(1, game.getGameMap().getHeight() - 4);
    std::uniform_int_distribution<> distrib_frame(0, 3);
    for (int i = 0; i < game.getSpriteCount().acid_drops; ++i) {
        while (true)
        {
            MapPos pos = {distrib_x(random_engine), distrib_y(random_engine), 1, 0};
            if (!game.getGameMap().isStone(pos.cellPos(), 0, -1) &&
                game.getGameMap().getBlock(pos.cellPos(), 0, -1) != GameMap::Cell::TRAPDOOR)
                continue;
            if (game.getGameMap().isStone(pos.cellPos()))
                continue;
            if (!game.getGameMap().isStone(pos.cellPos(), 0, 1))
                continue;
            if (auto cell = game.getGameMap().getBlock(pos.cellPos(), -1);
                cell == GameMap::Cell::ROPE_END ||
                cell == GameMap::Cell::ROPE_MAIN ||
                cell == GameMap::Cell::ROPE_START)
                continue;
            if (auto cell = game.getGameMap().getBlock(pos.cellPos(), 1);
                cell == GameMap::Cell::ROPE_END ||
                cell == GameMap::Cell::ROPE_MAIN ||
                cell == GameMap::Cell::ROPE_START)
                continue;
            auto initial_state = distrib_frame(random_engine);
            sprite_list.emplace_back(std::make_unique<AcidDrop>(pos, initial_state));
            break;
        }
    }
}

void AcidDrop::update(unsigned)
{
    // State 5-Check Position
    if (sprite_pos.cy() == 0)
    {                       // State 4-Check Grow
        if (frame_id != 5)
            ++frame_id;     // State 2-Grow
        else
        {                   // State 7-Start Fall
            frame_id = 4;
            sprite_pos.yadd(1);
        }
    }
    else if (sprite_pos.cy() == 4)
    {                       // State 6-Check Splash
        if (frame_id != 6)
            frame_id = 6;   // State 3-Splash
        else
        {                   // State 8-Reset
            frame_id = 0;
            sprite_pos.yadd(-4);
        }
    }
    else
        sprite_pos.yadd(1); // State 9-Fall
}


Sprite::BoxParams const &AcidDrop::getBoxParams() const
{
    static BoxParams box_params[] = {
        {      1,     9, {6,  0,  4,  2}, {}},
        {   26+1,     9, {6,  0,  5,  4}, {}},
        { 2*26+1,     9, {6,  0,  5,  5}, {}},
        { 3*26+1,     9, {6,  0,  5,  6}, {}},
        {      1,  36+9, {5, -1,  7,  7}, {}},
        {   26+1,  36+9, {5,  0,  7,  9}, {{6, 0, 5, 8}}},
        { 2*26+1,  36+9, {0,  0, 16,  8}, {}},
    };
    assert(frame_id < std::size(box_params));
    return box_params[frame_id];
}

Sprite::RenderParams AcidDrop::getRenderParams() const
{
    return {TextureID::ACID_DROP, false, {}};
}

Label AcidDrop::getLabel() const
{
    auto label_index = static_cast<unsigned>(LabelID::ACID_DROP1) + frame_id;
    assert(label_index <= static_cast<unsigned>(LabelID::ACID_DROP7));
    return labelOf(static_cast<LabelID>(label_index));
}
