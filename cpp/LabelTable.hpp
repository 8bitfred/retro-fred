#pragma once

#include "Label.hpp"
#include "Coord.hpp"
#include "sdl.hpp"
#include <vector>
#include <algorithm>

class LabelTable
{
    SDL_Rect window_rect;
    SDL_Rect char_rect;
    std::vector<Label> data;

public:
    explicit LabelTable(SDL_Rect const &window_rect);
    void reset() { std::fill(data.begin(), data.end(), 0); }
    void set(SDL_Rect const &rect, Label v);
};
