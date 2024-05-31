#pragma once

#include "fredcore/Sprite.hpp"

class GameBase;

class Bullet: public Sprite {
    GameBase &game;
    int direction;
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;

public:
    static constexpr int MAX_DISTANCE = 10;
    Bullet(GameBase &game, MapPos initial_position, int direction)
        : Sprite(initial_position), game(game), direction(direction) {}

    void update(unsigned) override { sprite_pos.xadd(2 * direction); }
    Label getLabel() const override { return labelOf(LabelID::BULLET); }
    bool isAlive();
};
