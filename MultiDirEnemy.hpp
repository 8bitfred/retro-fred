#pragma once

#include "Sprite.hpp"
#include <random>

class GameMap;

class MultiDirEnemy : public Sprite
{
public:
    MultiDirEnemy(Window const &window, MapPos const &pos,
                  int char_width, int char_height,
                  std::minstd_rand &random_engine)
        : Sprite(window, pos, char_width, char_height), random_engine(random_engine) {}

protected:
    enum Direction
    {
        DIRECTION_LEFT,
        DIRECTION_DOWN,
        DIRECTION_RIGHT,
        DIRECTION_UP,
        DIRECTION_COUNT
    };

    std::pair<int, int> getDirDelta() const;
    bool getRandomSense();
    void selectDirection(GameMap const &game_map, bool clockwise);

    std::minstd_rand &random_engine;
    Direction direction = DIRECTION_LEFT;
};
