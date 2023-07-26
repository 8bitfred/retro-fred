#pragma once

#include "MultiDirEnemy.hpp"
#include <random>

class GameMap;

class Skeleton : public MultiDirEnemy
{
public:
    Skeleton(Frame const &frame, MapPos const &pos, std::minstd_rand &random_engine)
    : MultiDirEnemy(frame, pos, 4, 4, random_engine) {}

    void update(Game &game, unsigned events) override;
    static void toggleClimbingFrame() { climbing_frame ^= 1; }

protected:
    RenderInfo getTexture() const override;

private:
    enum class FrameType
    {
        STANDING,
        BIG_STEP,
        SMALL_STEP,
        CLIMBING1,
        CLIMBING2,
    };
    static int climbing_frame;
    FrameType frame_type = FrameType::STANDING;
};
