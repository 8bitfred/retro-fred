#pragma once

#include "Sprite.hpp"
#include <random>

class GameMap;

class Mummy : public Sprite
{
public:
    Mummy(Game& game, std::minstd_rand &random_engine);

    void update(Game &game, unsigned events) override;
    static void toggleMummyTimer() { mummy_timer ^= 1; }

protected:
    RenderInfo getTexture() const override;

private:
    static MapPos getRandomLocation(std::minstd_rand &random_engine, 
                                    GameMap const &game_map);
    static int getRandomDirection(std::minstd_rand &random_engine);

    void stateWalk(Game &game);
    void stateFall(Game &game);
    void stateBounce(Game &game);
    void stateDisappear(Game &game);


    enum class Frame
    {
        STEP,
        STANDING,
        FALL,
        POP,
    };
    enum class State
    {
        WALK,
        FALL,
        BOUNCE,
        DISAPPEAR,
    };
    static int mummy_timer;
    State state = State::WALK;
    std::minstd_rand &random_engine;
    int direction;
    Frame frame = Frame::STEP;
    bool flip = false;
};
