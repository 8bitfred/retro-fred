#include "Smoke.hpp"
#include <cassert>

Sprite::BoxParams const &Smoke::getBoxParams() const
{
    static BoxParams box_params[3] = {
        {      9, 9, {-1, -1, 26, 18}, {} },
        {   50+9, 9, {-1, -1, 26, 18}, {} },
        { 2*50+9, 9, {-1, -1, 26, 18}, {} },
    };
    assert(state < 3);
    return box_params[state];
}

Sprite::RenderParams Smoke::getRenderParams() const
{
    return {TextureID::SMOKE, false, {}};
}
