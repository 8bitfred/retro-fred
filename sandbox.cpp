#include "sdl.hpp"
#include "Signal.hpp"
#include <iostream>


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

int main()
{
    test_signals();

    return 0;
}