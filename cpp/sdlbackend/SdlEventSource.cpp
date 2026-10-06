#include "SdlEventSource.hpp"
#include "fredcore/Controller.hpp"
#include <cstdlib>

namespace {
    struct KeyBinding
    {
        Uint16 keymod;
        SDL_Scancode scancode;
        GameEvent game_event;
    };

    KeyBinding game_bindings[] = {
        { SDL_KMOD_CTRL,  SDL_SCANCODE_Q,      GameEvent::QUIT },
        { SDL_KMOD_CTRL,  SDL_SCANCODE_W,      GameEvent::BACK },
        { SDL_KMOD_NONE,  SDL_SCANCODE_AC_BACK,GameEvent::BACK },
        { SDL_KMOD_NONE,  SDL_SCANCODE_ESCAPE, GameEvent::ESCAPE },
        { SDL_KMOD_NONE,  SDL_SCANCODE_LEFT,   GameEvent::LEFT },
        { SDL_KMOD_NONE,  SDL_SCANCODE_RIGHT,  GameEvent::RIGHT },
        { SDL_KMOD_NONE,  SDL_SCANCODE_UP,     GameEvent::UP },
        { SDL_KMOD_NONE,  SDL_SCANCODE_DOWN,   GameEvent::DOWN },
        { SDL_KMOD_NONE,  SDL_SCANCODE_SPACE,  GameEvent::FIRE },
        { SDL_KMOD_NONE,  SDL_SCANCODE_RETURN, GameEvent::RETURN },
        { SDL_KMOD_SHIFT, SDL_SCANCODE_LEFT,   GameEvent::DBG_LEFT },
        { SDL_KMOD_SHIFT, SDL_SCANCODE_RIGHT,  GameEvent::DBG_RIGHT },
        { SDL_KMOD_SHIFT, SDL_SCANCODE_UP,     GameEvent::DBG_UP },
        { SDL_KMOD_SHIFT, SDL_SCANCODE_DOWN,   GameEvent::DBG_DOWN },
        { SDL_KMOD_SHIFT, SDL_SCANCODE_F,      GameEvent::DBG_CENTER_FRED },
        { SDL_KMOD_SHIFT, SDL_SCANCODE_R,      GameEvent::DBG_CENTER_WINDOW },
        { SDL_KMOD_SHIFT, SDL_SCANCODE_H,      GameEvent::DBG_MOVE_TO_EXIT },
        { SDL_KMOD_SHIFT, SDL_SCANCODE_O,      GameEvent::DBG_EXIT_LEFT },
        { SDL_KMOD_SHIFT, SDL_SCANCODE_P,      GameEvent::DBG_EXIT_RIGHT },
        { SDL_KMOD_SHIFT, SDL_SCANCODE_K,      GameEvent::DBG_DIE },
        { SDL_KMOD_SHIFT, SDL_SCANCODE_M,      GameEvent::DBG_MAP },
    };
}

std::optional<GameEvent> SdlEventSource::getTouchEvent(Controller const &virtual_controller,
                                                       SDL_TouchFingerEvent const &tfinger)
{
    auto const &arrows_rect = virtual_controller.getArrowRect();
    auto arrows_x = tfinger.x - arrows_rect.x;
    auto arrows_y = tfinger.y - arrows_rect.y;
    if (arrows_x >= 0 && arrows_x < arrows_rect.w &&
        arrows_y >= 0 && arrows_y < arrows_rect.h)
    {
        auto x = arrows_x - (arrows_rect.w / 2);
        auto y = arrows_y - (arrows_rect.h / 2);
        if (std::abs(x) > std::abs(y))
        {
            if (x > 0)
                return GameEvent::RIGHT;
            else
                return GameEvent::LEFT;
        }
        else
        {
            if (y > 0)
                return GameEvent::DOWN;
            else
                return GameEvent::UP;
        }
    }

    FPoint point{tfinger.x, tfinger.y};
    if (pointInRect(point, virtual_controller.getFireRect()))
        return GameEvent::FIRE;

    if (virtual_controller.hasBackButton())
    {
        if (pointInRect(point, virtual_controller.getBackRect()))
            return GameEvent::BACK;
    }
    return {};
}

void SdlEventSource::tryAddJoystick(SDL_JoystickID device_id)
{
    if (joystick || gamepad)
        return;
    if (SDL_IsGamepad(device_id))
        gamepad.emplace(SDL_OpenGamepad(device_id));
    else {
        auto candidate = sdl::JoystickPtr(SDL_OpenJoystick(device_id));
        joystick.emplace(SDL_OpenJoystick(device_id));
        if (SDL_GetNumJoystickAxes(candidate) >= 2 && SDL_GetNumJoystickButtons(candidate) >= 2)
            joystick = std::move(candidate);
    }
}

void SdlEventSource::tryRemoveJoystick(SDL_JoystickID instance_id)
{
    if (gamepad) {
        auto joystick_ptr = SDL_GetGamepadJoystick(gamepad->get());
        if (SDL_GetJoystickID(joystick_ptr) == instance_id)
            gamepad.reset();
    }
    else if (joystick) {
        if (SDL_GetJoystickID(joystick->get()) == instance_id)
            joystick.reset();
    }

}


void SdlEventSource::getJoystickHatEvent(EventMask &event_mask, Uint8 hat_position)
{
    switch (hat_position)
    {
        case SDL_HAT_DOWN:
            event_mask.set(GameEvent::DOWN);
            break;
        case SDL_HAT_UP:
            event_mask.set(GameEvent::UP);
            break;
        case SDL_HAT_LEFT:
            event_mask.set(GameEvent::LEFT);
            break;
        case SDL_HAT_RIGHT:
            event_mask.set(GameEvent::RIGHT);
            break;
        case SDL_HAT_LEFTDOWN:
            event_mask.set(GameEvent::LEFT);
            event_mask.set(GameEvent::DOWN);
            break;
        case SDL_HAT_LEFTUP:
            event_mask.set(GameEvent::LEFT);
            event_mask.set(GameEvent::UP);
            break;
        case SDL_HAT_RIGHTDOWN:
            event_mask.set(GameEvent::RIGHT);
            event_mask.set(GameEvent::DOWN);
            break;
        case SDL_HAT_RIGHTUP:
            event_mask.set(GameEvent::RIGHT);
            event_mask.set(GameEvent::UP);
            break;
    }
}

void SdlEventSource::getJoystickButtonEvent(EventMask &event_mask, Uint8 button) {
    if (button == 0 || button >= 4)
        event_mask.set(GameEvent::FIRE);
    else if (button == 1)
        event_mask.set(GameEvent::BACK);
}

void SdlEventSource::getAxisEvent(EventMask &event_mask, Sint16 x, Sint16 y)
{
    auto absx = std::abs(x);
    auto absy = std::abs(y);
    if (absx < 10000)
        absx = 0;
    if (absy < 10000)
        absy = 0;
    if (absx > absy)
    {
        if (x > 0)
            event_mask.set(GameEvent::RIGHT);
        else
            event_mask.set(GameEvent::LEFT);
    }
    else if (absx < absy)
    {
        if (y > 0)
            event_mask.set(GameEvent::DOWN);
        else
            event_mask.set(GameEvent::UP);
    }
}

void SdlEventSource::getJoystickAxisEvent(EventMask &event_mask)
{
    if (!joystick)
        return;
    auto pjoystick = joystick->get();
    auto num_axes = std::min(SDL_GetNumJoystickAxes(pjoystick), 4);
    for (int axis = 1; axis < num_axes; axis += 2)
    {
        getAxisEvent(event_mask,
                     SDL_GetJoystickAxis(pjoystick, axis - 1),
                     SDL_GetJoystickAxis(pjoystick, axis));
    }
}

void SdlEventSource::getGamepadButtonEvent(EventMask &event_mask)
{
    auto pgamepad = gamepad->get();
    if (SDL_GetGamepadButton(pgamepad, SDL_GAMEPAD_BUTTON_SOUTH))
            event_mask.set(GameEvent::FIRE);
    if (SDL_GetGamepadButton(pgamepad, SDL_GAMEPAD_BUTTON_EAST) ||
        SDL_GetGamepadButton(pgamepad, SDL_GAMEPAD_BUTTON_BACK))
        event_mask.set(GameEvent::BACK);
    if (SDL_GetGamepadButton(pgamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT))
        event_mask.set(GameEvent::LEFT);
    if (SDL_GetGamepadButton(pgamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT))
        event_mask.set(GameEvent::RIGHT);
    if (SDL_GetGamepadButton(pgamepad, SDL_GAMEPAD_BUTTON_DPAD_UP))
        event_mask.set(GameEvent::UP);
    if (SDL_GetGamepadButton(pgamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN))
        event_mask.set(GameEvent::DOWN);
}

void SdlEventSource::getGamepadAxisEvent(EventMask &event_mask)
{
    if (!gamepad)
        return;
    auto pgamepad = gamepad->get();
    getAxisEvent(event_mask,
                 SDL_GetGamepadAxis(pgamepad, SDL_GAMEPAD_AXIS_LEFTX),
                 SDL_GetGamepadAxis(pgamepad, SDL_GAMEPAD_AXIS_LEFTY));
    getAxisEvent(event_mask,
                 SDL_GetGamepadAxis(pgamepad, SDL_GAMEPAD_AXIS_RIGHTX),
                 SDL_GetGamepadAxis(pgamepad, SDL_GAMEPAD_AXIS_RIGHTY));
}


EventMask SdlEventSource::collectEvents(std::optional<Controller> const &virtual_controller)
{
    static constexpr Uint16 STDMODS = SDL_KMOD_SHIFT | SDL_KMOD_ALT | SDL_KMOD_CTRL | SDL_KMOD_GUI;
    EventMask event_mask;
    while (true)
    {
        SDL_Event event;
        if (timer_expiration && *timer_expiration < next_frame)
        {
            auto timeout = static_cast<std::int32_t>(*timer_expiration - static_cast<Uint32>(SDL_GetTicks()));
            if (timeout <= 0 || !SDL_WaitEventTimeout(&event, timeout))
            {
                timer_expiration.reset();
                event_mask.set(GameEvent::TIMER);
                break;
            }
        }
        else {
            auto timeout = static_cast<std::int32_t>(next_frame - static_cast<Uint32>(SDL_GetTicks()));
            if (timeout <= 0 || !SDL_WaitEventTimeout(&event, timeout))
            {
                next_frame = static_cast<Uint32>(SDL_GetTicks()) + ticks_per_frame;
                break;
            }
        }
        if (event.type == SDL_EVENT_QUIT)
        {
            event_mask.set(GameEvent::QUIT);
            break;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN)
        {
            Uint16 keymods = event.key.mod & STDMODS;
            if (keymods == 0)
                event_mask.set(GameEvent::ANY_KEY);
            for (auto const &binding: game_bindings)
            {
                if (checkKeymod(keymods, binding.keymod) &&
                    event.key.scancode == binding.scancode)
                {
                    event_mask.set(binding.game_event);
                    if (binding.game_event == GameEvent::QUIT)
                        return event_mask;
                    break;
                }
            }
        }
        else if (event.type == SDL_EVENT_JOYSTICK_ADDED)
            tryAddJoystick(event.jdevice.which);
        else if (event.type == SDL_EVENT_JOYSTICK_REMOVED)
            tryRemoveJoystick(event.jdevice.which);

        if (joystick) {
            if (event.type == SDL_EVENT_JOYSTICK_HAT_MOTION)
                getJoystickHatEvent(event_mask, event.jhat.hat);
            else if (event.type == SDL_EVENT_JOYSTICK_BUTTON_DOWN)
                getJoystickButtonEvent(event_mask, event.jbutton.button);
            else if (event.type == SDL_EVENT_JOYSTICK_AXIS_MOTION && event.jaxis.axis < 2)
                getJoystickAxisEvent(event_mask);
        }
        if (gamepad) {
            if (event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION)
                getGamepadAxisEvent(event_mask);
            else if (event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN)
                getGamepadButtonEvent(event_mask);
        }
        if (virtual_controller)
        {
            if (event.type == SDL_EVENT_FINGER_DOWN)
            {
                auto game_event = getTouchEvent(*virtual_controller, event.tfinger);
                // Trigger the ANY_KEY event only if the event is for the center of the
                // screen (20%-80% of the x coordinates). This avoids triggering the
                // ANY_KEY event when the user is trying to make the BACK gesture.
                if (event.tfinger.x > .2 && event.tfinger.x < .8)
                    event_mask.set(GameEvent::ANY_KEY);
                if (game_event)
                {
                    event_mask.set(*game_event);
                    finger_state[event.tfinger.fingerID] = *game_event;
                }
            }
            else if (event.type == SDL_EVENT_FINGER_UP)
                finger_state.erase(event.tfinger.fingerID);
            else if (event.type == SDL_EVENT_FINGER_MOTION)
            {
                auto game_event = getTouchEvent(*virtual_controller, event.tfinger);
                auto state = finger_state.find(event.tfinger.fingerID);
                if (state != finger_state.end() && game_event)
                    state->second = *game_event;
            }
        }
    }
    auto keystate = SDL_GetKeyboardState(nullptr);
    Uint16 keymods = SDL_GetModState() & STDMODS;
    for (auto const &binding : game_bindings)
    {
        if (checkKeymod(keymods, binding.keymod) &&
            keystate[binding.scancode])
            event_mask.set(binding.game_event);
    }
    for (auto const &[id, game_event] : finger_state)
        event_mask.set(game_event);

    if (joystick) {
        auto pjoystick = joystick->get();
        auto num_hats = SDL_GetNumJoystickHats(pjoystick);
        for (int hat = 0; hat < num_hats; ++hat)
            getJoystickHatEvent(event_mask, SDL_GetJoystickHat(pjoystick, hat));
        Uint8 num_buttons = static_cast<Uint8>(SDL_GetNumJoystickButtons(pjoystick));
        for (Uint8 button = 0; button < num_buttons; ++button)
        {
            if (SDL_GetJoystickButton(pjoystick, button))
                getJoystickButtonEvent(event_mask, button);
        }
        getJoystickAxisEvent(event_mask);
    }
    if (gamepad) {
        getGamepadButtonEvent(event_mask);
        getGamepadAxisEvent(event_mask);
    }

    return event_mask;
}
