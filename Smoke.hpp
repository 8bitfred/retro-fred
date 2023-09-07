#pragma once

#include "Sprite.hpp"

class Smoke : public Sprite
{
public:
    Smoke(Window const &window, MapPos const &pos)
    : Sprite::Sprite(window, pos, 5, 4) {}

    void update(Game &, unsigned) override { ++state; }
    bool isAlive() const { return state < 3; }

protected:
    RenderInfo const &getTexture() const override;

private:
    int state = 0;
};
