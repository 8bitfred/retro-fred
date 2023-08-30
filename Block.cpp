#include "Block.hpp"

Block::Block(Window const &window, MapPos const &pos,
             TextureID texture_id)
             : Sprite(window, pos, MapPos::CELL_WIDTH, MapPos::CELL_HEIGHT)
             , texture_id(texture_id)
{
}
