#pragma once
#include <cstdint>

enum class SoundID : std::uint8_t
{
    PICK_OBJECT,
    COLLISION,
    FIRE,
    CLIMB1,
    CLIMB2,
    JUMP,
    WALK,
    EXIT_MAZE,
    GAME_OVER,
    COUNT
};
