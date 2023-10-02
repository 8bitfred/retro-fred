#pragma once

#include "Sprite.hpp"

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
    void apply(Game &game) const;

private:
    Type type;
};
