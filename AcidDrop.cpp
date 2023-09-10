#include "AcidDrop.hpp"

void AcidDrop::update(Game &, unsigned)
{
    // State 4-Check Position
    if (sprite_pos.cy() == 0)
    {                       // State 3-Check Frame
        if (frame_id != 5)
            ++frame_id;     // State 2-Grow
        else
        {                   // State 6-Start Fall
            frame_id = 4;
            sprite_pos.yadd(1);
        }
    }
    else if (sprite_pos.cy() == 4)
    {                       // State 5-Check Splash
        if (frame_id == 4)
            frame_id = 6;   // State 8-Splash
        else
        {                   // State 9-Reset
            frame_id = 0;
            sprite_pos.yadd(-4);
        }
    }
    else
        sprite_pos.yadd(1); // State 7-Fall
}


Sprite::BoxParams const &AcidDrop::getBoxParams() const
{
    static BoxParams box_params[] = {
        {      1,     8, {5, 0,  7, 10}, {}},
        {   26+1,     8, {5, 0,  7, 10}, {}},
        { 2*26+1,     8, {5, 0,  7, 10}, {}},
        { 3*26+1,     8, {5, 0,  7, 10}, {}},
        {      1,  36+8, {5, 0,  7, 10}, {}},
        {   26+1,  36+8, {5, 0,  7, 10}, {}},
        { 2*26+1,  36+8, {0, 0, 16, 10}, {}},
    };
    assert(frame_id < std::size(box_params));
    return box_params[frame_id];
}

Sprite::RenderParams AcidDrop::getRenderParams() const
{
    return {TextureID::ACID_DROP, false, {}};
}
