#include "Tomb.hpp"

Sprite::BoxParams const &Tomb::getBoxParams() const
{
    static Sprite::BoxParams box_params = {1, 1, {0, 0, 72, 41}, {}};
    return box_params;
}

Sprite::RenderParams Tomb::getRenderParams() const
{
    return {TextureID::TOMB, false, {}};
}
