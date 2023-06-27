#include "Block.hpp"

Block::Block(Frame const &frame, MapPos const &pos,
             TextureID texture_id)
             : Sprite(frame, pos, MapPos::CELL_WIDTH, MapPos::CELL_HEIGHT)
             , texture_id(texture_id)
{
}
