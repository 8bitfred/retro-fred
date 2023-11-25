#pragma once

#include "sdl.hpp"
#include <optional>

enum class GameEvent
{
    QUIT,
    ANY_KEY,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    FIRE,
    DBG_LEFT,
    DBG_RIGHT,
    DBG_UP,
    DBG_DOWN,
    DBG_CENTER_FRED,
    DBG_CENTER_WINDOW,
    DBG_MOVE_TO_EXIT,
    DBG_EXIT_LEFT,
    DBG_EXIT_RIGHT,
    DBG_DIE,
    DBG_MAP,
    TIMER,
    COUNT,
};

class EventMask {
    std::uint32_t event_mask;

public:
    explicit EventMask(std::uint32_t event_mask = 0) : event_mask(event_mask) {}
    void set(GameEvent game_event)
    { 
        auto bit_number = static_cast<int>(game_event);
        event_mask |= 1 << bit_number;
    }
    bool check(GameEvent game_event) const
    {
        int bit_number = static_cast<int>(game_event);
        return (event_mask & (1 << bit_number)) != 0;
    }
};

class EventManager {
    std::uint32_t ticks_per_frame;
    Uint32 next_frame;
    std::optional<Uint32> timer_expiration;

    static bool checkKeymod(Uint16 keymod, Uint16 mask)
    {
        return (keymod & ~mask) == 0 && (mask == 0 || (keymod & mask) != 0);
    }

public:
    explicit EventManager(std::uint32_t ticks_per_frame)
    : ticks_per_frame(ticks_per_frame)
    , next_frame(SDL_GetTicks() + ticks_per_frame)
    {}
    EventMask collectEvents();
    void setTimer(std::uint32_t ticks)
    {
        timer_expiration = SDL_GetTicks() + ticks;
    }
};
