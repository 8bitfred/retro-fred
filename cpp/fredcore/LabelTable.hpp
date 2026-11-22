#pragma once

#include "Label.hpp"
#include "Coord.hpp"
#include "platform/Geometry.hpp"
#include <vector>
#include <algorithm>
#include <string>

class LabelTable
{
    Rect char_rect;
    std::vector<Label> data;

public:
    explicit LabelTable(Rect const &window_rect);
    void reset() { std::fill(data.begin(), data.end(), 0); }
    void set(Rect const &rect, Label v);
    Label get(int x, int y) const { return data[y * char_rect.w + x]; }
    std::string toString() const;
};
