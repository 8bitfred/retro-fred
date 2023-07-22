#pragma once
#include <cstdint>

enum class TextureID : std::uint8_t
{
    FRAME_BASE,

    BLOCK_STONE1,
    BLOCK_STONE2,
    BLOCK_STONE3,
    BLOCK_ROPE_START,
    BLOCK_ROPE_MAIN,
    BLOCK_ROPE_END,
    BLOCK_SKY,
    BLOCK_SAND,
    BLOCK_TRAPDOOR,

    ACID_DROP,
    RAT,
    GHOST,
    FRED,
    COUNT
};
