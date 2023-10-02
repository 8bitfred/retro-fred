#pragma once

#include "Sprite.hpp"

class Game;

class Bullet: public Sprite {
    Game &game;
    int direction;
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;

public:
    static constexpr int MAX_DISTANCE = 10;
    Bullet(Game &game, MapPos initial_position, int direction)
        : Sprite(initial_position), game(game), direction(direction) {}

    void update(unsigned) override { sprite_pos.xadd(2 * direction); }
    bool isAlive();
};
