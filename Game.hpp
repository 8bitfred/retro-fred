#pragma once

#include "Frame.hpp"
#include "GameMap.hpp"

class Game
{
public:
    Game(Config const &cfg, std::minstd_rand &random_engine);

private:
    Frame frame;
    GameMap game_map;
};
