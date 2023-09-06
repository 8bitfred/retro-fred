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
    CHAMELEON,
    MUMMY,
    VAMPIRE,
    SKELETON,
    BULLET,
    SMOKE,
    FRED,
    FRED_RED,
    FRED_CYAN,
    FRED_GREEN,
    FRED_MAGENTA,
    FRED_BLUE,
    COUNT
};
