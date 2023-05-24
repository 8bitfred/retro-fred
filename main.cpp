#include <iostream>
#include "sdl.hpp"
#include "Config.hpp"
#include "GameMap.hpp"
#include "Application.hpp"

int main(int argc, char *argv[])
{
    std::minstd_rand random_engine;
    const Config cfg;
    GameMap gmap(random_engine, cfg.map_width, cfg.map_height);

    FredApp fred_app(cfg);
    auto renderer = fred_app.getRenderer();

    SDL_Rect dest{cfg.frame_x, cfg.frame_y, cfg.frame_w, cfg.frame_h};
    SDL_RenderClear(renderer);
    gmap.render(cfg.map_x, cfg.map_y, fred_app.getTextureManager(), fred_app.getRenderer(), &dest);
    SDL_RenderPresent(renderer);

    while (1)
    {
        SDL_Event event;
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT)
            break;
    }

    return 0;
}
