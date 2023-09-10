#pragma once

#include "Sprite.hpp"
#include "GameMap.hpp"

class Block: public Sprite {
    GameMap::Cell cell;

public:
    Block(MapPos const &pos, GameMap::Cell cell)
        : Sprite(pos), cell(cell) {}

protected:
    virtual RenderParams getRenderParams() const override;
    virtual BoxParams &getBoxParams() const override;
};
