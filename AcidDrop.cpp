#include "AcidDrop.hpp"

AcidDrop::StateInfo const &AcidDrop::getStateInfo() const
{
    // Offset of cells 0, 1, 2 and 3:
    static constexpr int x_c0 = 1;
    static constexpr int x_c1 = x_c0 + 26;
    static constexpr int x_c2 = x_c1 + 26;
    static constexpr int x_c3 = x_c2 + 26;

    static constexpr int y_c0 = 8;
    static constexpr int y_c1 = y_c0 + 36;

    static StateInfo state_info[] = {
        /*  0 */ {{TextureID::ACID_DROP, {x_c0+5, y_c0,  7, 10}, -5, 1}, 0},
        /*  1 */ {{TextureID::ACID_DROP, {x_c1+5, y_c0,  7, 10}, -5, 1}, 0},
        /*  2 */ {{TextureID::ACID_DROP, {x_c2+5, y_c0,  7, 10}, -5, 1}, 0},
        /*  3 */ {{TextureID::ACID_DROP, {x_c3+5, y_c0,  7, 10}, -5, 1}, 0},
        /*  4 */ {{TextureID::ACID_DROP, {x_c0+5, y_c1,  7, 10}, -5, 1}, 0},
        /*  5 */ {{TextureID::ACID_DROP, {x_c1+5, y_c1,  7, 10}, -5, 1}, 1},
        /*  6 */ {{TextureID::ACID_DROP, {x_c0+5, y_c1,  7, 10}, -5, 1}, 1},
        /*  7 */ {{TextureID::ACID_DROP, {x_c0+5, y_c1,  7, 10}, -5, 1}, 1},
        /*  8 */ {{TextureID::ACID_DROP, {x_c0+5, y_c1,  7, 10}, -5, 1}, 1},
        /*  9 */ {{TextureID::ACID_DROP, {x_c0+5, y_c1,  7, 10}, -5, 1}, 0},
        /* 10 */ {{TextureID::ACID_DROP, {x_c2,   y_c1, 16, 10},  0, 1}, -4},
    };

    return state_info[state];
}

void AcidDrop::update(Game &, unsigned)
{
    sprite_pos.yadd(getStateInfo().ydelta);
    state = (state + 1) % 11;
}


Sprite::RenderInfo const &AcidDrop::getTexture() const
{
    return getStateInfo().render_info;
}