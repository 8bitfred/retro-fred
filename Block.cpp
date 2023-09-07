#include "Block.hpp"

Block::Block(Window const &window, MapPos const &pos,
             TextureID texture_id)
             : Sprite(window, pos, MapPos::CELL_WIDTH, MapPos::CELL_HEIGHT)
             , render_info{texture_id, {0, 0, 32, 40}, 0, 0}
{
}
