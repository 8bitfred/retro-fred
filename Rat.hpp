#pragma once

#include "Sprite.hpp"

class GameMap;

class Rat : public Sprite
{
    GameMap const &game_map;
    int direction = 1;
    int alternate_frame = 1;

    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
public:
    Rat(GameMap const &game_map, MapPos const &pos)
    : Sprite::Sprite(pos), game_map(game_map) {}

    void update(unsigned events) override;
};
