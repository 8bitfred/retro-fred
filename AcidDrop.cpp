#include "AcidDrop.hpp"

void AcidDrop::update(Game &, unsigned)
{
    if (texture_id == TextureID::ACID_DROP11)
        texture_id = TextureID::ACID_DROP1;
    else
        texture_id = static_cast<TextureID>(static_cast<int>(texture_id) + 1);
}
