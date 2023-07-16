#pragma once

#include "Sprite.hpp"

class Block: public Sprite {
public:
    Block(Frame const &frame, MapPos const &pos, TextureID texture_id);

protected:
    RenderInfo getTexture() const override
    {
        return {texture_id, {0, 0, 32, 40}, 0, 0};
    }

private:
    TextureID texture_id;
};
