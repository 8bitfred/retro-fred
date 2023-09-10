#pragma once

#include "Sprite.hpp"

class AcidDrop : public Sprite
{
    enum State
    {
        GROW,
        FALL,
        SPLASH,
        RESET,
    };
    State state = State::GROW;
    unsigned frame_id = 0;

public:
    AcidDrop(MapPos const &pos, unsigned frame_id)
    : Sprite::Sprite(pos), frame_id(frame_id) {}

    void update(Game &game, unsigned events) override;

protected:
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
};
