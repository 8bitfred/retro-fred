#pragma once

#include "Sprite.hpp"

class AcidDrop : public Sprite
{
public:
    AcidDrop(Window const &window, MapPos const &pos, int initial_state)
    : Sprite::Sprite(window, pos, 2, 1), state(initial_state) {}

    void update(Game &game, unsigned events) override;

protected:
    RenderInfo const &getTexture() const override;

private:
    struct StateInfo {
        RenderInfo render_info;
        int ydelta;
    };
    StateInfo const &getStateInfo() const;
    int state = 0;
};
