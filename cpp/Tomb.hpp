#pragma once

#include "Sprite.hpp"

class Tomb : public Sprite
{
    BoxParams const &getBoxParams() const override;
    RenderParams getRenderParams() const override;
    Label getLabel() const override { return labelOf(LabelID::TOMB); }

public:
    explicit Tomb(MapPos pos): Sprite(pos) {}
};
