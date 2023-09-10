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

    void moveSlow(Game &game);
    void moveStatic(Game &game);
    void moveFast(Game &game);

    int alternate_frame = 0;
    Mode mode = Mode::SLOW;

public:
    Vampire(MapPos const &pos, std::minstd_rand &random_engine);

    BulletEffect bulletHit() override { return BulletEffect::DIE; }
    void update(Game &game, unsigned events) override;

protected:
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
};
