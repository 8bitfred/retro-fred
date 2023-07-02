#pragma once

#include "Sprite.hpp"

class AcidDrop : public Sprite
{
public:
    AcidDrop(Frame const &frame, MapPos const &pos, TextureID initial_texture_id)
    : Sprite::Sprite(frame, pos, 4, 5), texture_id(initial_texture_id) {}

    void update(Game &game, unsigned events) override;

protected:
    std::pair<TextureID, CenterPos> getTexture() const override
    {
        return {texture_id, {}};
    }

private:
    TextureID texture_id = TextureID::ACID_DROP1;
};
