#pragma once

#include "MultiDirEnemy.hpp"
#include <random>

class GameMap;

class Vampire : public MultiDirEnemy
{
public:
    Vampire(Window const &window, MapPos const &pos, std::minstd_rand &random_engine);

    BulletEffect bulletHit() override { return BulletEffect::DIE; }
    void update(Game &game, unsigned events) override;

protected:
    RenderInfo getTexture() const override;

private:
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
};
