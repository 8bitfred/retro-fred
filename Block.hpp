#pragma once

#include "Sprite.hpp"

class Block: public Sprite {
public:
    Block(Window const &window, MapPos const &pos, TextureID texture_id);

protected:
    RenderInfo const &getTexture() const override { return render_info; }

private:
    RenderInfo render_info;
};
