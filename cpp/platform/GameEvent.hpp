#pragma once

#include <cstdint>
#include <optional>

class Controller;

enum class GameEvent
{
    QUIT,
    BACK,
    ESCAPE,
    ANY_KEY,
    LEFT,
    RIGHT,
    UP,
    DOWN,
    FIRE,
    RETURN,
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

// Collects input events (keyboard, joystick, game controller, touch, timer) from the
// underlying platform and translates them into GameEvents.
class EventSource
{
public:
    virtual ~EventSource() = default;
    virtual EventMask collectEvents(std::optional<Controller> const &virtual_controller) = 0;
    virtual void setTimer(std::uint32_t ticks) = 0;
};
