#include "sdl.hpp"
#include <iostream>

int main(int, char *[])
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
