#include "AcidDrop.hpp"

void AcidDrop::update(unsigned)
{
    // State 5-Check Position
    if (sprite_pos.cy() == 0)
    {                       // State 4-Check Grow
        if (frame_id != 5)
            ++frame_id;     // State 2-Grow
        else
        {                   // State 7-Start Fall
            frame_id = 4;
            sprite_pos.yadd(1);
        }
    }
    else if (sprite_pos.cy() == 4)
    {                       // State 6-Check Splash
        if (frame_id != 6)
            frame_id = 6;   // State 3-Splash
        else
        {                   // State 8-Reset
            frame_id = 0;
            sprite_pos.yadd(-4);
        }
    }
    else
        sprite_pos.yadd(1); // State 9-Fall
}


Sprite::BoxParams const &AcidDrop::getBoxParams() const
{
    static BoxParams box_params[] = {
        {      1,     9, {6,  0,  4,  2}, {}},
        {   26+1,     9, {6,  0,  5,  4}, {}},
        { 2*26+1,     9, {6,  0,  5,  5}, {}},
        { 3*26+1,     9, {6,  0,  5,  6}, {}},
        {      1,  36+9, {5, -1,  7,  7}, {}},
        {   26+1,  36+9, {5,  0,  7,  9}, {{6, 0, 5, 8}}},
        { 2*26+1,  36+9, {0,  0, 16,  8}, {}},
    };
    assert(frame_id < std::size(box_params));
    return box_params[frame_id];
}

Sprite::RenderParams AcidDrop::getRenderParams() const
{
    return {TextureID::ACID_DROP, false, {}};
}
