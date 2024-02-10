#include "LabelTable.hpp"

LabelTable::LabelTable(SDL_Rect const &window_rect)
    : window_rect(window_rect)
{
    char_rect.x = window_rect.x / MapPos::PIXELS_PER_CHAR;
    auto right = ceil_of_div(window_rect.x + window_rect.w, MapPos::PIXELS_PER_CHAR);
    char_rect.w = right - char_rect.x;
    char_rect.y = window_rect.y / MapPos::PIXELS_PER_CHAR;
    auto bottom = ceil_of_div(window_rect.y + window_rect.h, MapPos::PIXELS_PER_CHAR);
    char_rect.h = bottom - char_rect.y;
    data.resize(char_rect.w * char_rect.h, 0);
}

void LabelTable::set(SDL_Rect const &rect, Label v)
{
    for (int dx = 0; dx <= rect.w; dx += MapPos::PIXELS_PER_CHAR)
    {
        int x = (rect.x + dx) / 8;
        if (x < char_rect.x || x >= (char_rect.x + char_rect.w))
            continue;
        for (int dy = 0; dy <= rect.h; dy += MapPos::PIXELS_PER_CHAR)
        {
            int y = (rect.y + dy) / 8;
            if (y < char_rect.y || y >= (char_rect.y + char_rect.h))
                continue;
            data[y * char_rect.w + x] |= v;
        }
    }
}
