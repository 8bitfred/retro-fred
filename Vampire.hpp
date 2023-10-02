#pragma once

#include "MultiDirEnemy.hpp"
#include <random>

class GameMap;

class Vampire : public MultiDirEnemy
{
    enum class Mode
    {
        SLOW,
        STATIC,
        FAST,
    };

    GameMap const &game_map;
    int alternate_frame = 0;
    Mode mode = Mode::SLOW;

    void moveSlow();
    void moveStatic();
    void moveFast();
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;

public:
    Vampire(GameMap const &game_map, MapPos const &pos, std::minstd_rand &random_engine);

    BulletEffect bulletHit() override { return BulletEffect::DIE; }
    void update(unsigned events) override;
};
