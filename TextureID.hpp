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

    ACID_DROP1,
    ACID_DROP2,
    ACID_DROP3,
    ACID_DROP4,
    ACID_DROP5,
    ACID_DROP6,
    ACID_DROP7,
    ACID_DROP8,
    ACID_DROP9,
    ACID_DROP10,
    ACID_DROP11,

    FRED,
    COUNT
};
