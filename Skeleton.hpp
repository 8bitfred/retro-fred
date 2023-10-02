#pragma once

#include "MultiDirEnemy.hpp"
#include <random>

class Game;

class Skeleton : public MultiDirEnemy
{
    enum class Frame
    {
        STANDING,
        BIG_STEP,
        SMALL_STEP,
        CLIMBING1,
        CLIMBING2,
        COUNT,
    };
    static int climbing_frame;
    Game &game;
    Frame frame = Frame::STANDING;

    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
public:
    Skeleton(Game &game, MapPos const &pos, std::minstd_rand &random_engine)
    : MultiDirEnemy(pos, random_engine), game(game) {}

    void update(unsigned events) override;
    BulletEffect bulletHit() override { return BulletEffect::DIE; }
    static void toggleClimbingFrame() { climbing_frame ^= 1; }
};
