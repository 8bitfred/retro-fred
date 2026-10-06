#pragma once

#include "fredcore/Sprite.hpp"

class Smoke : public Sprite
{
    int state = 0;
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
    Label getLabel() const override;
public:
    explicit Smoke(MapPos pos): Sprite(pos) {}
    void update(unsigned) override { ++state; }
    bool isAlive() const { return state < 3; }
};
