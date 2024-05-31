#pragma once

#include "fredcore/Sprite.hpp"
#include <random>

class GameBase;

class Object : public Sprite
{
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
    Label getLabel() const override;

public:
    enum class Type
    {
        MAP,
        POWER,
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
    static void initialize(std::minstd_rand &random_engine, GameBase &game);
    void apply(GameBase &game) const;

private:
    Type type;
};
