#pragma once

#include "Sprite.hpp"

class Rat : public Sprite
{
    int direction = 1;
    int alternate_frame = 0;
public:
    explicit Rat(MapPos const &pos)
    : Sprite::Sprite(pos) {}

    void update(Game &game, unsigned events) override;

protected:
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
};
