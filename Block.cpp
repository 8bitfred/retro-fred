#include "Block.hpp"

Block::Block(Frame const &frame, MapPos const &pos,
             SDL_Texture *texture)
             : Sprite(frame, pos, MapPos::CELL_WIDTH, MapPos::CELL_HEIGHT)
             , texture(texture)
{
}

std::pair<SDL_Texture *, Sprite::CenterPos> Block::getTexture() const
{
    return {texture, {}};
}
