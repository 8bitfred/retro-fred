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

Label Smoke::getLabel() const
{
    return labelOf(static_cast<LabelID>(static_cast<int>(LabelID::SMOKE1) + state));
}