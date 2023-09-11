#pragma once

#include "Sprite.hpp"

class Object : public Sprite
{
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

protected:
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;

private:
    Type type;
};
