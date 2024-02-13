#pragma once

#include "Sprite.hpp"
#include <random>

class GameBase;
class GameMap;

class Ghost : public Sprite
{
    struct Direction {
        int x = 0, y = 0;
        bool operator==(Direction const &) const = default;
    };
    GameMap const &game_map;
    std::minstd_rand &random_engine;
    int alternate_frame = 0;
    Direction direction;

    void setRandomDirection();
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
    Label getLabel() const override;

public:
    Ghost(GameMap const &game_map, MapPos const &pos, std::minstd_rand &random_engine);
    static void initialize(std::minstd_rand &random_engine, GameBase &game);
    void update(unsigned events) override;
    BulletEffect bulletHit() override
    {
        direction.x *= -1;
        direction.y *= -1;
        return BulletEffect::HIT;
    }
};
