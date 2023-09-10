#pragma once

#include "Sprite.hpp"
#include <random>

class GameMap;

class Chameleon : public Sprite
{
    struct Direction {
        int x = -1, y = -1;
    };
    std::minstd_rand &random_engine;
    int alternate_frame = 0;
    Direction direction;
public:
    Chameleon(MapPos const &pos, std::minstd_rand &random_engine)
        : Sprite::Sprite(pos), random_engine(random_engine) {}

    void update(Game &game, unsigned events) override;

    static bool isValidCell(GameMap const &game_map, CellPos const &pos);

protected:
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
};
