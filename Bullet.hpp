#pragma once

#include "Sprite.hpp"

class Bullet: public Sprite {
public:
    static constexpr int MAX_DISTANCE = 10;
    Bullet(Window const &window, MapPos initial_position, int direction)
        : Sprite(window, initial_position, 3, 1), direction(direction) {}

    void update(Game &, unsigned) override { sprite_pos.xadd(2 * direction); }
    bool isAlive(Game &game);

protected:
    RenderInfo getTexture() const override
    {
        return {TextureID::BULLET, {1, 1, 24, 16}, 0, 0};
    }

private:
    int direction;
};
