#pragma once

#include "Sprite.hpp"
#include <random>

class GameMap;

class Mummy : public Sprite
{
public:
    Mummy(Game& game, std::minstd_rand &random_engine);

    void update(Game &game, unsigned events) override;

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
    void stateReappear(Game &game);


    enum class FrameType
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
        REAPPEAR,
    };
    State state = State::WALK;
    std::minstd_rand &random_engine;
    int frame_dir;
    FrameType frame_type = FrameType::STEP;
    int mummy_timer = 0;
    bool flip = false;
};
