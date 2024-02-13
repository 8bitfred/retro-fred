#pragma once

#include "MultiDirEnemy.hpp"
#include <random>

class GameBase;

class Skeleton : public MultiDirEnemy
{
    enum class Frame
    {
        STANDING,
        BIG_STEP,
        SMALL_STEP,
        CLIMBING_CLAMPING,
        CLIMBING_EXTENDING,
        COUNT,
    };
    static int climbing_frame;
    GameBase &game;
    Frame frame = Frame::STANDING;

    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
    Label getLabel() const override;
public:
    Skeleton(GameBase &game, MapPos const &pos, std::minstd_rand &random_engine)
    : MultiDirEnemy(pos, random_engine), game(game) {}
    static void initialize(std::minstd_rand &random_engine, GameBase &game);
    void update(unsigned events) override;
    BulletEffect bulletHit() override { return BulletEffect::DIE; }
    static void toggleClimbingFrame() { climbing_frame ^= 1; }
};
