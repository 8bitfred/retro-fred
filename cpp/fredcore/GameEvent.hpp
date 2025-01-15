#pragma once

#include "sdl.hpp"
#include <optional>
#include <map>

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

class EventManager {
    std::uint32_t ticks_per_frame;
    Uint32 next_frame;
    std::optional<Uint32> timer_expiration;
    std::optional<sdl::JoystickPtr> joystick;

    static bool checkKeymod(Uint16 keymod, Uint16 mask)
    {
        return (keymod & ~mask) == 0 && (mask == 0 || (keymod & mask) != 0);
    }

    std::map<SDL_FingerID, GameEvent> finger_state;
    std::optional<GameEvent> getTouchEvent(Controller const &virtual_controller,
                                           SDL_TouchFingerEvent const &tfinger);
    static void getJoystickHatEvent(EventMask &event_mask, Uint8 hat_position);
    static void getJoystickButtonEvent(EventMask &event_mask, Uint8 button);
    void getJoystickAxisEvent(EventMask &event_mask);

public:
    explicit EventManager(std::uint32_t ticks_per_frame);
    EventMask collectEvents(std::optional<Controller> const &virtual_controller);
    void setTimer(std::uint32_t ticks)
    {
        timer_expiration = SDL_GetTicks() + ticks;
    }
};
