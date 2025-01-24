#include "GameEvent.hpp"
#include "Controller.hpp"
#include "SDL_gamecontroller.h"

namespace {
    struct KeyBinding
    {
        Uint16 keymod;
        SDL_Scancode scancode;
        GameEvent game_event;
    };

    KeyBinding game_bindings[] = {
        { KMOD_CTRL,  SDL_SCANCODE_Q,      GameEvent::QUIT },
        { KMOD_CTRL,  SDL_SCANCODE_W,      GameEvent::BACK },
        { KMOD_NONE,  SDL_SCANCODE_AC_BACK,GameEvent::BACK },
        { KMOD_NONE,  SDL_SCANCODE_ESCAPE, GameEvent::ESCAPE },
        { KMOD_NONE,  SDL_SCANCODE_LEFT,   GameEvent::LEFT },
        { KMOD_NONE,  SDL_SCANCODE_RIGHT,  GameEvent::RIGHT },
        { KMOD_NONE,  SDL_SCANCODE_UP,     GameEvent::UP },
        { KMOD_NONE,  SDL_SCANCODE_DOWN,   GameEvent::DOWN },
        { KMOD_NONE,  SDL_SCANCODE_SPACE,  GameEvent::FIRE },
        { KMOD_NONE,  SDL_SCANCODE_RETURN, GameEvent::RETURN },
        { KMOD_SHIFT, SDL_SCANCODE_LEFT,   GameEvent::DBG_LEFT },
        { KMOD_SHIFT, SDL_SCANCODE_RIGHT,  GameEvent::DBG_RIGHT },
        { KMOD_SHIFT, SDL_SCANCODE_UP,     GameEvent::DBG_UP },
        { KMOD_SHIFT, SDL_SCANCODE_DOWN,   GameEvent::DBG_DOWN },
        { KMOD_SHIFT, SDL_SCANCODE_F,      GameEvent::DBG_CENTER_FRED },
        { KMOD_SHIFT, SDL_SCANCODE_R,      GameEvent::DBG_CENTER_WINDOW },
        { KMOD_SHIFT, SDL_SCANCODE_H,      GameEvent::DBG_MOVE_TO_EXIT },
        { KMOD_SHIFT, SDL_SCANCODE_O,      GameEvent::DBG_EXIT_LEFT },
        { KMOD_SHIFT, SDL_SCANCODE_P,      GameEvent::DBG_EXIT_RIGHT },
        { KMOD_SHIFT, SDL_SCANCODE_K,      GameEvent::DBG_DIE },
        { KMOD_SHIFT, SDL_SCANCODE_M,      GameEvent::DBG_MAP },
    };
}

std::optional<GameEvent> EventManager::getTouchEvent(Controller const &virtual_controller,
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

    SDL_FPoint point{tfinger.x, tfinger.y};
    if (SDL_PointInFRect(&point, &virtual_controller.getFireRect()))
        return GameEvent::FIRE;

    if (virtual_controller.hasBackButton())
    {
        if (SDL_PointInFRect(&point, &virtual_controller.getBackRect()))
            return GameEvent::BACK;
    }
    return {};
}

void EventManager::tryAddJoystick(int device_index)
{
    if (joystick || game_controller)
        return;
    if (SDL_IsGameController(device_index))
        game_controller.emplace(SDL_GameControllerOpen(device_index));
    else {
        auto candidate = sdl::JoystickPtr(SDL_JoystickOpen(device_index));
        joystick.emplace(SDL_JoystickOpen(device_index));
        if (SDL_JoystickNumAxes(candidate) >= 2 && SDL_JoystickNumButtons(candidate) >= 2)
            joystick = std::move(candidate);
    }
}

void EventManager::tryRemoveJoystick(int instance_id)
{
    if (game_controller) {
        auto joystick_ptr = SDL_GameControllerGetJoystick(game_controller->get());
        if (SDL_JoystickInstanceID(joystick_ptr) == instance_id)
            game_controller.reset();
    }
    else if (joystick) {
        if (SDL_JoystickInstanceID(joystick->get()) == instance_id)
            joystick.reset();
    }

}


void EventManager::getJoystickHatEvent(EventMask &event_mask, Uint8 hat_position)
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

void EventManager::getJoystickButtonEvent(EventMask &event_mask, Uint8 button) {
    if (button == 0 || button >= 4)
        event_mask.set(GameEvent::FIRE);
    else if (button == 1)
        event_mask.set(GameEvent::BACK);
}

void EventManager::getAxisEvent(EventMask &event_mask, Sint16 x, Sint16 y)
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

void EventManager::getJoystickAxisEvent(EventMask &event_mask)
{
    if (!joystick)
        return;
    auto pjoystick = joystick->get();
    auto num_axes = std::min(SDL_JoystickNumAxes(pjoystick), 4);
    for (int axis = 1; axis < num_axes; axis += 2)
    {
        getAxisEvent(event_mask,
                     SDL_JoystickGetAxis(pjoystick, axis - 1),
                     SDL_JoystickGetAxis(pjoystick, axis));
    }
}

void EventManager::getGameControllerButtonEvent(EventMask &event_mask)
{
    auto pgame_controller = game_controller->get();
    if (SDL_GameControllerGetButton(pgame_controller, SDL_CONTROLLER_BUTTON_A) == 1)
            event_mask.set(GameEvent::FIRE);
    if (SDL_GameControllerGetButton(pgame_controller, SDL_CONTROLLER_BUTTON_B) == 1 ||
        SDL_GameControllerGetButton(pgame_controller, SDL_CONTROLLER_BUTTON_BACK) == 1)
        event_mask.set(GameEvent::BACK);
    if (SDL_GameControllerGetButton(pgame_controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT) == 1)
        event_mask.set(GameEvent::LEFT);
    if (SDL_GameControllerGetButton(pgame_controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == 1)
        event_mask.set(GameEvent::RIGHT);
    if (SDL_GameControllerGetButton(pgame_controller, SDL_CONTROLLER_BUTTON_DPAD_UP) == 1)
        event_mask.set(GameEvent::UP);
    if (SDL_GameControllerGetButton(pgame_controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN) == 1)
        event_mask.set(GameEvent::DOWN);
}

void EventManager::getGameControllerAxisEvent(EventMask &event_mask)
{
    if (!game_controller)
        return;
    auto pgame_controller = game_controller->get();
    getAxisEvent(event_mask,
                 SDL_GameControllerGetAxis(pgame_controller, SDL_CONTROLLER_AXIS_LEFTX),
                 SDL_GameControllerGetAxis(pgame_controller, SDL_CONTROLLER_AXIS_LEFTY));
    getAxisEvent(event_mask,
                 SDL_GameControllerGetAxis(pgame_controller, SDL_CONTROLLER_AXIS_RIGHTX),
                 SDL_GameControllerGetAxis(pgame_controller, SDL_CONTROLLER_AXIS_RIGHTY));
}


EventMask EventManager::collectEvents(std::optional<Controller> const &virtual_controller)
{
    static constexpr Uint16 STDMODS = KMOD_SHIFT | KMOD_ALT | KMOD_CTRL | KMOD_GUI;
    EventMask event_mask;
    while (true)
    {
        SDL_Event event;
        if (timer_expiration && *timer_expiration < next_frame)
        {
            auto timeout = static_cast<std::int32_t>(*timer_expiration - SDL_GetTicks());
            if (timeout <= 0 || SDL_WaitEventTimeout(&event, timeout) == 0)
            {
                timer_expiration.reset();
                event_mask.set(GameEvent::TIMER);
                break;
            }
        }
        else {
            auto timeout = static_cast<std::int32_t>(next_frame - SDL_GetTicks());
            if (timeout <= 0 || SDL_WaitEventTimeout(&event, timeout) == 0)
            {
                next_frame = SDL_GetTicks() + ticks_per_frame;
                break;
            }
        }
        if (event.type == SDL_QUIT)
        {
            event_mask.set(GameEvent::QUIT);
            break;
        }
        else if (event.type == SDL_KEYDOWN)
        {
            Uint16 keymods = event.key.keysym.mod & STDMODS;
            if (keymods == 0)
                event_mask.set(GameEvent::ANY_KEY);
            for (auto const &binding: game_bindings)
            {
                if (checkKeymod(keymods, binding.keymod) &&
                    event.key.keysym.scancode == binding.scancode)
                {
                    event_mask.set(binding.game_event);
                    if (binding.game_event == GameEvent::QUIT)
                        return event_mask;
                    break;
                }
            }
        }
        else if (event.type == SDL_JOYDEVICEADDED)
            tryAddJoystick(event.jdevice.which);
        else if (event.type == SDL_JOYDEVICEREMOVED)
            tryRemoveJoystick(event.jdevice.which);

        if (joystick) {
            if (event.type == SDL_JOYHATMOTION)
                getJoystickHatEvent(event_mask, event.jhat.hat);
            else if (event.type == SDL_JOYBUTTONDOWN)
                getJoystickButtonEvent(event_mask, event.jbutton.button);
            else if (event.type == SDL_JOYAXISMOTION && event.jaxis.axis < 2)
                getJoystickAxisEvent(event_mask);
        }
        if (game_controller) {
            if (event.type == SDL_CONTROLLERAXISMOTION)
                getGameControllerAxisEvent(event_mask);
            else if (event.type == SDL_CONTROLLERBUTTONDOWN)
                getGameControllerButtonEvent(event_mask);
        }
        if (virtual_controller)
        {
            if (event.type == SDL_FINGERDOWN)
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
                    finger_state[event.tfinger.fingerId] = *game_event;
                }
            }
            else if (event.type == SDL_FINGERUP)
                finger_state.erase(event.tfinger.fingerId);
            else if (event.type == SDL_FINGERMOTION)
            {
                auto game_event = getTouchEvent(*virtual_controller, event.tfinger);
                auto state = finger_state.find(event.tfinger.fingerId);
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
            keystate[binding.scancode] != 0)
            event_mask.set(binding.game_event);
    }
    for (auto const &[id, game_event] : finger_state)
        event_mask.set(game_event);

    if (joystick) {
        auto pjoystick = joystick->get();
        auto num_hats = SDL_JoystickNumHats(pjoystick);
        for (int hat = 0; hat < num_hats; ++hat)
            getJoystickHatEvent(event_mask, SDL_JoystickGetHat(pjoystick, hat));
        Uint8 num_buttons = static_cast<Uint8>(SDL_JoystickNumButtons(pjoystick));
        for (Uint8 button = 0; button < num_buttons; ++button)
        {
            if (SDL_JoystickGetButton(pjoystick, button) == 1)
                getJoystickButtonEvent(event_mask, button);
        }
        getJoystickAxisEvent(event_mask);
    }
    if (game_controller) {
        getGameControllerButtonEvent(event_mask);
        getGameControllerAxisEvent(event_mask);
    }

    return event_mask;
}
