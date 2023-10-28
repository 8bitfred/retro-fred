#pragma once

#include "Sprite.hpp"
#include <random>

class Game;

class Object : public Sprite
{
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;

public:
    enum class Type
    {
        MAP,
        LIFE,
        BUST,
        STONE,
        BULLETS,
        STATUE,
        MASK,
        AMULET,
        COUNT
    };
    Object(MapPos const &pos, Type type)
        : Sprite::Sprite(pos), type(type) {}
    static void initialize(std::minstd_rand &random_engine, Game &game);
    void apply(Game &game) const;

private:
    Type type;
};
