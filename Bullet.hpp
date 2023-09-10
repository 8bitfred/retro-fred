#pragma once

#include "Sprite.hpp"

class Bullet: public Sprite {
    int direction;
public:
    static constexpr int MAX_DISTANCE = 10;
    Bullet(MapPos initial_position, int direction)
        : Sprite(initial_position), direction(direction) {}

    void update(Game &, unsigned) override { sprite_pos.xadd(2 * direction); }
    bool isAlive(Game &game);

protected:
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
};
