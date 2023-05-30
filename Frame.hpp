#pragma once

#include "Coord.hpp"

struct Config;

class Frame
{
public:
    static constexpr int SCORE_WIDTH = 7;
    explicit Frame(Config const &cfg);

private:
    MapPos gFrame;
    // Position of the game window
    ScreenPos p1, p2;
    ScreenPos gFredPos;
    ScreenPos gFramePos;
};
