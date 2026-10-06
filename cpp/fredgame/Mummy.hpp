#pragma once

#include "fredcore/Sprite.hpp"
#include <random>

class GameMap;
class GameBase;

class Mummy : public Sprite
{
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
    GameMap const &game_map;
    std::minstd_rand &random_engine;
    State state = State::WALK;
    int direction;
    Frame frame = Frame::STANDING;
    bool flip = false;

    static MapPos getRandomLocation(std::minstd_rand &random_engine, 
                                    GameMap const &game_map);
    static int getRandomDirection(std::minstd_rand &random_engine);

    void stateWalk();
    void stateFall();
    void stateBounce();
    void stateDisappear();
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
    Label getLabel() const override;

public:
    Mummy(GameMap const& game_map, std::minstd_rand &random_engine);
    static void initialize(std::minstd_rand &random_engine, GameBase &game);
    void update(unsigned events) override;
    BulletEffect bulletHit() override;
    static void toggleMummyTimer() { mummy_timer ^= 1; }
};
