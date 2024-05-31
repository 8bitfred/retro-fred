#pragma once

#include "fredcore/Sprite.hpp"
#include <random>

class GameBase;

class AcidDrop : public Sprite
{
    unsigned frame_id = 0;
public:
    AcidDrop(MapPos const &pos, unsigned frame_id)
    : Sprite::Sprite(pos), frame_id(frame_id) {}

    static void initialize(std::minstd_rand &random_engine, GameBase &game);
    void update(unsigned events) override;

protected:
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
    Label getLabel() const override;
};
