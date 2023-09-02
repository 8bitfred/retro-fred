#pragma once

#include "Sprite.hpp"

class Bullet: public Sprite {
public:
    static constexpr int MAX_DISTANCE = 10;
    Bullet(Window const &window, MapPos initial_position, int direction)
        : Sprite(window, initial_position, 3, 2), direction(direction) {}

    void update(Game &, unsigned) override
    {
        sprite_pos.xadd(2*direction);
        distance += 2;
    }

    bool maxDistance() const { return distance >= MAX_DISTANCE; }

protected:
    RenderInfo getTexture() const override
    {
        return {TextureID::BULLET, {1, 1, 24, 16}, 0, 0};
    }

private:
    int direction;
    int distance = 0;
};
