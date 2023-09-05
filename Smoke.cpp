#include "Smoke.hpp"
#include <cassert>

Sprite::RenderInfo Smoke::getTexture() const
{
    // Offset of cells 0, 1, 2 and 3:
    static constexpr int c0 = 8;
    static constexpr int c1 = c0 + 50;
    static constexpr int c2 = c1 + 50;
    static RenderInfo textures[3] =
        {
            {TextureID::SMOKE, {c0, 8, 26, 18}, 1, 1}, // frame id 0
            {TextureID::SMOKE, {c1, 8, 26, 18}, 1, 1}, // frame id 1
            {TextureID::SMOKE, {c2, 8, 26, 18}, 1, 1}, // frame id 0
        };
    assert(state < 3);
    return textures[state];
}
