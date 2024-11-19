#include "fredcore/sdl.hpp"
#include "Signal.hpp"
#include "fredcore/GameEvent.hpp"
#include "fredcore/Controller.hpp"
#include <iostream>


// SDL_GetKeyboardState
// SDL_SCANCODE_...
// https://wiki.libsdl.org/SDL2/SDL_GetModState

void test_signals()
{
    Signal<void, int> sigtest;

    std::vector<Connection> vec;
    vec.reserve(10);

    vec.emplace_back(sigtest.connect([&vec](int x)
                                     {
                                        std::cout << "fn0: " << x << std::endl;
                                        if (vec.size() > 1)
                                            vec.erase(vec.begin());
                                     }));
    vec.emplace_back(sigtest.connect([](int x)
                                     { std::cout << "fn1: " << x << std::endl; }));
    vec.emplace_back(sigtest.connect([](int x)
                                     { std::cout << "fn2: " << x << std::endl; }));
    sigtest(5);
    // if (vec.size() > 1)
    //     vec.erase(vec.begin() + 1);

    //vec.pop_back();
    //vec.erase(vec.begin());
    std::cout << "vec.size=" << vec.size() << std::endl;
    sigtest(6);
    vec.front().disconnect();
    sigtest(7);
}

int test_events()
{
    using namespace std::string_view_literals;
    sdl::App test_app;
    auto [window, renderer] = sdl::createWindowAndRenderer(1000, 1000);

    SDL_Event event;
    while (SDL_WaitEvent(&event))
    {
        if (event.type == SDL_QUIT)
            return 0;
        std::string_view type;
        if (event.type == SDL_KEYDOWN)
            type = "KEYDOWN"sv;
        else if (event.type == SDL_KEYUP)
            type = "KEYUP  "sv;
        else
            continue;
        std::cout << type << " " << SDL_GetKeyName(event.key.keysym.sym)
                  << " rep=" << std::dec << static_cast<bool>(event.key.repeat)
                  << " mod=" << std::hex << event.key.keysym.mod
                  << std::endl;
    }
    std::cout << "event error: " << SDL_GetError() << std::endl;
    return 1;
}

void test_event_manager()
{
    using namespace std::string_view_literals;
    auto event_codes = "QALRUDFlrudcxhopkmT"sv;

    sdl::App test_app;
    auto [window, renderer] = sdl::createWindowAndRenderer(1000, 1000);
    EventManager event_manager(1000);
    event_manager.setTimer(5200);
    auto ticks = SDL_GetTicks();
    while (true)
    {
        auto event_mask = event_manager.collectEvents({});
        auto new_ticks = SDL_GetTicks();
        std::string event_str;
        bool any_event = false;
        for (size_t i = 0; i < event_codes.size(); ++i)
        {
            if (i > 0 && (i % 4) == 0)
                event_str += " ";
            if (event_mask.check(static_cast<GameEvent>(i)))
            {
                event_str += event_codes[i];
                any_event = true;
            }
            else
                event_str += '_';
        }
        if (any_event)
        {
            std::cout << "delta_ticks=" << new_ticks - ticks << std::endl;
            std::cout << "  event=" << event_str << std::endl;
        }
        if (event_mask.check(GameEvent::QUIT))
            break;
    }
}

int main(int, char *[])
{
    test_event_manager();

    return 0;
}