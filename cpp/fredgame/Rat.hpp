#pragma once

#include "fredcore/Sprite.hpp"
#include <random>

class GameMap;
class GameBase;

class Rat : public Sprite
{
    GameMap const &game_map;
    int direction = 1;
    int alternate_frame = 1;

    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
    Label getLabel() const override;
public:
    Rat(GameMap const &game_map, MapPos const &pos)
    : Sprite::Sprite(pos), game_map(game_map) {}

    static void initialize(std::minstd_rand &random_engine, GameBase &game);
    void update(unsigned events) override;
};
