#pragma once

#include "platform/GameEvent.hpp"
#include "sdl_raii.hpp"
#include <map>

class SdlEventSource : public EventSource
{
    std::uint32_t ticks_per_frame;
    Uint32 next_frame;
    std::optional<Uint32> timer_expiration;
    std::optional<sdl::JoystickPtr> joystick;
    std::optional<sdl::GamepadPtr> gamepad;
    std::map<SDL_FingerID, GameEvent> finger_state;

    static bool checkKeymod(Uint16 keymod, Uint16 mask)
    {
        return (keymod & ~mask) == 0 && (mask == 0 || (keymod & mask) != 0);
    }

    std::optional<GameEvent> getTouchEvent(Controller const &virtual_controller,
                                           SDL_TouchFingerEvent const &tfinger);
    void tryAddJoystick(SDL_JoystickID device_id);
    void tryRemoveJoystick(SDL_JoystickID instance_id);
    static void getJoystickHatEvent(EventMask &event_mask, Uint8 hat_position);
    static void getJoystickButtonEvent(EventMask &event_mask, Uint8 button);
    static void getAxisEvent(EventMask &event_mask, Sint16 x, Sint16 y);
    void getJoystickAxisEvent(EventMask &event_mask);
    void getGamepadButtonEvent(EventMask &event_mask);
    void getGamepadAxisEvent(EventMask &event_mask);

public:
    explicit SdlEventSource(std::uint32_t ticks_per_frame)
        : ticks_per_frame(ticks_per_frame)
        , next_frame(static_cast<Uint32>(SDL_GetTicks()) + ticks_per_frame) {}
    EventMask collectEvents(std::optional<Controller> const &virtual_controller) override;
    void setTimer(std::uint32_t ticks) override
    {
        timer_expiration = static_cast<Uint32>(SDL_GetTicks()) + ticks;
    }
};
