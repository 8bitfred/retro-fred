#pragma once

#include "Sprite.hpp"

class Rat : public Sprite
{
public:
    Rat(Window const &window, MapPos const &pos)
    : Sprite::Sprite(window, pos, 2, 1) {}

    void update(Game &game, unsigned events) override;

protected:
    RenderInfo getTexture() const override;

private:
    int frame_dir = 1;
    int frame_type = 0;
};
