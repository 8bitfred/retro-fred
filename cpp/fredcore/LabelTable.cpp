#include "LabelTable.hpp"

LabelTable::LabelTable(SDL_Rect const &window_rect)
{
    // TODO: I don't think this logic works correctly if window_rect.x + window_rect.w is
    // not a character border (the sum is 0 modulo 8)
    char_rect.x = window_rect.x / MapPos::PIXELS_PER_CHAR;
    auto right = ceil_of_div(window_rect.x + window_rect.w, MapPos::PIXELS_PER_CHAR);
    char_rect.w = right - char_rect.x;
    char_rect.y = window_rect.y / MapPos::PIXELS_PER_CHAR;
    auto bottom = ceil_of_div(window_rect.y + window_rect.h, MapPos::PIXELS_PER_CHAR);
    char_rect.h = bottom - char_rect.y;
    data.resize(char_rect.w * char_rect.h, 0);
}

constexpr int floor_div(int x, int y)
{
    return x / y + (x < 0 ? -1 : 0);
}

void LabelTable::set(SDL_Rect const &rect, Label v)
{
    // We assume that there is a black border around the sprite that does not need to
    // labeled. Another option would be to use the hit boxes for creating the labels.
    auto left = rect.x;
    auto right = rect.x + rect.w;
    if (rect.w > 2)
    {
        ++left;
        --right;
    }
    auto top = rect.y;
    auto bottom = rect.y + rect.h;
    if (rect.h > 2)
    {
        ++top;
        --bottom;
    }

    for (int char_y = floor_div(top, MapPos::PIXELS_PER_CHAR);
         (char_y * MapPos::PIXELS_PER_CHAR) < bottom; ++char_y)
    {
        if (char_y < char_rect.y || char_y >= (char_rect.y + char_rect.h))
            continue;
        for (int char_x = floor_div(left, MapPos::PIXELS_PER_CHAR);
             (char_x * MapPos::PIXELS_PER_CHAR) < right; ++char_x)
        {
            if (char_x < char_rect.x || char_x >= (char_rect.x + char_rect.w))
                continue;
            data[(char_y - char_rect.y) * char_rect.w + (char_x - char_rect.x)] |= v;
        }

    }
}

std::string LabelTable::toString() const
{
    static std::string label_str[] = {
        "rr", "ss", "oo", "xx",
        "o ",
        "A1", "A2", "A3", "A4", "A5", "A6", "A7",
        "Sl", "Sr", "Sc",
        "Ml", "Mr", "MF", "Mf", "Mp",
        "Vl", "Vr",
        "Rl", "Rr",
        "CL", "Cl", "CR", "Cr",
        "Om", "Op", "O3", "O4", "Ob", "O6", "O7", "O8",
        "Gl", "Gr",
        "s1", "s2", "s3",
        "FS", "Fs", "FB", "Fb", "FT", "Ft", "FC", "Fc",
        "Ff",
        "TT",
    };
    static_assert(std::size(label_str) == (static_cast<unsigned>(LabelID::MAX) + 1));

    std::string result;
    for (int y = 0; y < char_rect.h; ++y)
    {
        std::string line[2];
        for (int x = 0; x < char_rect.w; ++x)
        {
            auto bitmap = get(x, y);
            int index = static_cast<int>(std::size(label_str)) - 1;
            std::string buf[4] = {"  ", "  ", "  ", "  "};
            for (auto &b: buf)
            {
                for (; index >= 0; --index)
                {
                    auto bit = static_cast<Label>(1) << index;
                    if ((bitmap & bit) != 0)
                    {
                        b = label_str[index];
                        --index;
                        break;
                    }
                }
            }
            line[0] += buf[0] + buf[1] + '-';
            line[1] += buf[2] + buf[3] + ' ';
        }
        for (auto &l: line)
        {
            assert(!l.empty());
            l.back() = '\n';
            result += l;
        }
    }
    return result;
}
