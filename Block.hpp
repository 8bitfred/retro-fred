#pragma once

#include "Sprite.hpp"

class Block: public Sprite {
public:
    Block(Frame const &frame, MapPos const &pos, SDL_Texture *texture);

protected:
    std::pair<SDL_Texture *, CenterPos> getTexture() const override;

private:
    SDL_Texture *texture;
};
