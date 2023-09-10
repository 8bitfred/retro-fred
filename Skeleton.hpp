#pragma once

#include "MultiDirEnemy.hpp"
#include <random>

class GameMap;

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
    Frame frame = Frame::STANDING;

public:
    Skeleton(MapPos const &pos, std::minstd_rand &random_engine)
    : MultiDirEnemy(pos, random_engine) {}

    void update(Game &game, unsigned events) override;
    BulletEffect bulletHit() override { return BulletEffect::DIE; }
    static void toggleClimbingFrame() { climbing_frame ^= 1; }

protected:
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
};
