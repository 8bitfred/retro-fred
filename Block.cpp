#include "Block.hpp"

Sprite::BoxParams &Block::getBoxParams() const
{
    static BoxParams box_params[] = {
        {      1,      1, {0, 0, 32, 40}, {}},
        {   42+1,      1, {0, 0, 32, 40}, {}},
        { 2*42+1,      1, {0, 0, 32, 40}, {}},
        {      1,   50+1, {0, 0, 32, 40}, {}},
        {   42+1,   50+1, {0, 0, 32, 40}, {}},
        { 2*42+1,   50+1, {0, 0, 32, 40}, {}},
        {      1, 2*50+1, {0, 0, 32, 40}, {}},
        {   42+1, 2*50+1, {0, 0, 32, 40}, {}},
        { 2*42+1, 2*50+1, {0, 0, 32, 40}, {}},
    };
    static_assert(std::size(box_params) == static_cast<int>(GameMap::Cell::TRAPDOOR));
    assert(static_cast<int>(cell) > 0);
    assert(static_cast<int>(cell) <= static_cast<int>(GameMap::Cell::TRAPDOOR));
    return box_params[static_cast<int>(cell) - 1];
}

Sprite::RenderParams Block::getRenderParams() const
{
    return {TextureID::BLOCK, false, {}};
}
