#pragma once

#include <cstdint>
#include <algorithm>

struct Rect
{
    int x = 0, y = 0, w = 0, h = 0;
    bool operator==(Rect const &) const = default;
};

struct FRect
{
    float x = 0, y = 0, w = 0, h = 0;
};

struct FPoint
{
    float x = 0, y = 0;
};

struct Color
{
    std::uint8_t r = 0xff, g = 0xff, b = 0xff, a = 0xff;
};

inline bool hasIntersection(Rect const &a, Rect const &b)
{
    return a.x < b.x + b.w && b.x < a.x + a.w &&
           a.y < b.y + b.h && b.y < a.y + a.h;
}

inline bool intersectRect(Rect const &a, Rect const &b, Rect &out)
{
    if (!hasIntersection(a, b))
        return false;
    auto x1 = std::max(a.x, b.x);
    auto y1 = std::max(a.y, b.y);
    auto x2 = std::min(a.x + a.w, b.x + b.w);
    auto y2 = std::min(a.y + a.h, b.y + b.h);
    out = Rect{x1, y1, x2 - x1, y2 - y1};
    return true;
}

inline bool pointInRect(FPoint const &p, FRect const &r)
{
    return p.x >= r.x && p.x < r.x + r.w &&
           p.y >= r.y && p.y < r.y + r.h;
}
