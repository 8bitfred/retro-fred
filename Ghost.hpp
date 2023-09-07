#pragma once

#include "Sprite.hpp"
#include <random>

class Ghost : public Sprite
{
public:
    Ghost(Window const &window, MapPos const &pos, std::minstd_rand &random_engine);

    void update(Game &game, unsigned events) override;
    BulletEffect bulletHit() override
    {
        direction.x *= -1;
        direction.y *= -1;
        return BulletEffect::HIT;
    }

protected:
    RenderInfo const &getTexture() const override;

private:
    struct Direction {
        int x = 0, y = 0;
        bool operator==(Direction const &) const = default;
    };
    std::minstd_rand &random_engine;
    int alternate_frame = 0;
    Direction direction;

    void setRandomDirection();
};
