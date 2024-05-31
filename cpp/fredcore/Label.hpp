#pragma once

#include <cstdint>

enum class LabelID
{
    BLOCK_ROPE,
    BLOCK_STONE,
    BLOCK_OTHER,

    // Note that the exit cell will be labeled with both the BLOCK_EXIT and the BLOCK_ROPE
    // labels
    BLOCK_EXIT,

    BULLET,

    ACID_DROP1,
    ACID_DROP2,
    ACID_DROP3,
    ACID_DROP4,
    ACID_DROP5,
    ACID_DROP6,
    ACID_DROP7,

    SKELETON_LEFT,
    SKELETON_RIGHT,
    SKELETON_CLIMBING,

    MUMMY_LEFT,
    MUMMY_RIGHT,
    MUMMY_FALLING_LEFT,
    MUMMY_FALLING_RIGHT,
    MUMMY_POP,

    VAMPIRE_LEFT,
    VAMPIRE_RIGHT,

    RAT_LEFT,
    RAT_RIGHT,

    CHAMELEON_LEFT_DOWN,
    CHAMELEON_LEFT_UP,
    CHAMELEON_RIGHT_DOWN,
    CHAMELEON_RIGHT_UP,

    OBJECT_MAP,
    OBJECT_POWER,
    OBJECT_BUST,
    OBJECT_STONE,
    OBJECT_BULLETS,
    OBJECT_STATUE,
    OBJECT_MASK,
    OBJECT_AMULET,

    GHOST_LEFT,
    GHOST_RIGHT,

    SMOKE1,
    SMOKE2,
    SMOKE3,

    FRED_STANDING_LEFT,
    FRED_STANDING_RIGHT,
    FRED_BIG_STEP_LEFT,
    FRED_BIG_STEP_RIGHT,
    FRED_SMALL_STEP_LEFT,
    FRED_SMALL_STEP_RIGHT,
    FRED_CLIMBING_LEFT,
    FRED_CLIMBING_RIGHT,

    // The FRED_SHOOTING label will be applied in addition to any of the other fred labels
    // (standing, small step, big step) if Fred is shooting. Note that Fred cannot shoot
    // from a rope.
    FRED_SHOOTING,

    TOMB, // Only appears after Fred dies

    MAX = TOMB,
};

static_assert(static_cast<int>(LabelID::MAX) < 64);

using Label = std::uint64_t;

constexpr Label labelOf(LabelID label_id)
{
    return static_cast<std::uint64_t>(1) << static_cast<unsigned>(label_id);
}
