#pragma once

#include "Sprite.hpp"
#include <random>

class GameMap;
class Game;

class Chameleon : public Sprite
{
    struct Direction {
        int x = -1, y = -1;
    };

    GameMap const &game_map;
    std::minstd_rand &random_engine;
    int alternate_frame = 1;
    Direction direction;

    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
    Label getLabel() const override;
public:
    Chameleon(GameMap const &game_map, MapPos const &pos, std::minstd_rand &random_engine)
        : Sprite::Sprite(pos), game_map(game_map), random_engine(random_engine) {}
    static void initialize(std::minstd_rand &random_engine, Game &game);
    void update(unsigned events) override;

    static bool isValidCell(GameMap const &game_map, CellPos const &pos);
};
