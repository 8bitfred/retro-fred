#pragma once

#include "Sprite.hpp"

class AcidDrop : public Sprite
{
public:
    AcidDrop(Frame const &frame, MapPos const &pos, int initial_state)
    : Sprite::Sprite(frame, pos, 2, 1), state(initial_state) {}

    void update(Game &game, unsigned events) override;

protected:
    RenderInfo getTexture() const override;

private:
    struct StateInfo {
        RenderInfo render_info;
        int ydelta;
    };
    StateInfo getStateInfo() const;
    int state = 0;
};
