#include "GameEvent.hpp"

namespace {
    struct KeyBinding
    {
        SDL_Keymod keymod;
        SDL_Scancode scancode;
        GameEvent game_event;
    };

    KeyBinding game_bindings[] = {
        { KMOD_CTRL,  SDL_SCANCODE_Q,      GameEvent::QUIT },
        { KMOD_NONE,  SDL_SCANCODE_LEFT,   GameEvent::LEFT },
        { KMOD_NONE,  SDL_SCANCODE_RIGHT,  GameEvent::RIGHT },
        { KMOD_NONE,  SDL_SCANCODE_UP,     GameEvent::UP },
        { KMOD_NONE,  SDL_SCANCODE_DOWN,   GameEvent::DOWN },
        { KMOD_NONE,  SDL_SCANCODE_SPACE,  GameEvent::FIRE },
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

EventMask EventManager::collectEvents() 
{
    static constexpr int STDMODS = KMOD_SHIFT | KMOD_ALT | KMOD_CTRL | KMOD_GUI;
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
                next_frame += ticks_per_frame;
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
            auto keymods = event.key.keysym.mod & STDMODS;
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
    }
    auto keystate = SDL_GetKeyboardState(nullptr);
    auto keymods = SDL_GetModState() & STDMODS;
    for (auto const &binding : game_bindings)
    {
        if (checkKeymod(keymods, binding.keymod) &&
            keystate[binding.scancode] != 0)
        {
            event_mask.set(binding.game_event);
        }
    }

    return event_mask;
}
