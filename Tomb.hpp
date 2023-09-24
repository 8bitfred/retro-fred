#pragma once

#include "Sprite.hpp"

class GameMap;

class Tomb : public Sprite
{
public:
    explicit Tomb(MapPos pos): Sprite(pos) {}

protected:
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
};
