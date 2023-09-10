#pragma once

#include "MultiDirEnemy.hpp"
#include <random>

class GameMap;

class Vampire : public MultiDirEnemy
{
    enum class State
    {
        SLOW,
        STATIC,
        FAST,
    };

    void stateSlow(Game &game);
    void stateStatic(Game &game);
    void stateFast(Game &game);

    int alternate_frame = 0;
    State state = State::SLOW;

public:
    Vampire(MapPos const &pos, std::minstd_rand &random_engine);

    BulletEffect bulletHit() override { return BulletEffect::DIE; }
    void update(Game &game, unsigned events) override;

protected:
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
};
