#pragma once

#include "Sprite.hpp"
#include <random>

class GameMap;

class Vampire : public Sprite
{
public:
    Vampire(Frame const &frame, MapPos const &pos, std::minstd_rand &random_engine);

    void update(Game &game, unsigned events) override;

protected:
    RenderInfo getTexture() const override;

private:
    enum Direction
    {
        DIRECTION_LEFT,
        DIRECTION_DOWN,
        DIRECTION_RIGHT,
        DIRECTION_UP,
        DIRECTION_COUNT
    };
    enum class State
    {
        SLOW,
        STATIC,
        FAST,
    };

    std::pair<int, int> getDirDelta() const;
    void changeDirection(GameMap const& game_map);
    void stateSlow(Game &game);
    void stateStatic(Game &game);
    void stateFast(Game &game);

    std::minstd_rand &random_engine;
    Direction direction;
    int alternate_frame = 0;
    State state = State::SLOW;
};
