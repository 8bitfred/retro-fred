#pragma once

#include "Sprite.hpp"

class Block: public Sprite {
public:
    Block(Frame const &frame, MapPos const &pos, TextureID texture_id);

protected:
    std::pair<TextureID, CenterPos> getTexture() const override
    {
        return {texture_id, {}};
    }

private:
    TextureID texture_id;
};
