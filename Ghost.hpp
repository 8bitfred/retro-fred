#pragma once

#include "Sprite.hpp"
#include <random>

class Ghost : public Sprite
{
public:
    Ghost(Frame const &frame, MapPos const &pos, std::minstd_rand &random_engine);

    void update(Game &game, unsigned events) override;

protected:
    RenderInfo getTexture() const override;

private:
    struct Velocity {
        int x = 0, y = 0;
        bool operator==(Velocity const &) const = default;
    };
    std::minstd_rand &random_engine;
    int alternate_frame = 0;
    Velocity velocity;

    void setRandomDirection();
};
