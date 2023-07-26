#pragma once

#include "MultiDirEnemy.hpp"
#include <random>

class GameMap;

class Vampire : public MultiDirEnemy
{
public:
    Vampire(Frame const &frame, MapPos const &pos, std::minstd_rand &random_engine);

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
