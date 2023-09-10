#pragma once

#include "Sprite.hpp"

class Smoke : public Sprite
{
    int state = 0;
public:
    explicit Smoke(MapPos pos): Sprite(pos) {}
    void update(Game &, unsigned) override { ++state; }
    bool isAlive() const { return state < 3; }

protected:
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
};
