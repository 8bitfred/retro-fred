#pragma once

#include "MultiDirEnemy.hpp"
#include <random>

class GameMap;

class Skeleton : public MultiDirEnemy
{
public:
    Skeleton(Window const &window, MapPos const &pos, std::minstd_rand &random_engine)
    : MultiDirEnemy(window, pos, 4, 4, random_engine) {}

    void update(Game &game, unsigned events) override;
    BulletEffect bulletHit() override { return BulletEffect::DIE; }
    static void toggleClimbingFrame() { climbing_frame ^= 1; }

protected:
    RenderInfo getTexture() const override;

private:
    enum class Frame
    {
        STANDING,
        BIG_STEP,
        SMALL_STEP,
        CLIMBING1,
        CLIMBING2,
    };
    static int climbing_frame;
    Frame frame = Frame::STANDING;
};
