#pragma once

#include "Sprite.hpp"
#include <random>

class GameMap;

class Chameleon : public Sprite
{
public:
    Chameleon(Window const &window, MapPos const &pos, std::minstd_rand &random_engine);

    void update(Game &game, unsigned events) override;

    static bool isValidCell(GameMap const &game_map, CellPos const &pos);

protected:
    RenderInfo getTexture() const override;

private:
    std::minstd_rand &random_engine;
    int alternate_frame = 0;
    int direction_x = -1;
    int direction_y = -1;
};
