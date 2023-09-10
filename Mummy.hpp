#pragma once

#include "Sprite.hpp"
#include <random>

class GameMap;

class Mummy : public Sprite
{
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
        COUNT,
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
    Frame frame = Frame::STANDING;
    bool flip = false;

public:
    Mummy(Game& game, std::minstd_rand &random_engine);

    void update(Game &game, unsigned events) override;
    BulletEffect bulletHit() override { return BulletEffect::DIE; }
    static void toggleMummyTimer() { mummy_timer ^= 1; }

protected:
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
};
