#include <iostream>
#include "sdl.hpp"
#include "Config.hpp"
#include "GameMap.hpp"
#include "Application.hpp"
#include "Game.hpp"

int main(int, char*[])
{
    std::minstd_rand random_engine;
    const Config cfg;
    // GameMap gmap(random_engine, cfg.map_width, cfg.map_height);

    // Game game_test(cfg, random_engine);

    FredApp fred_app(cfg, random_engine);
    fred_app.playGame();
    // auto renderer = fred_app.getRenderer();

    // SDL_Rect dest{0, 0, cfg.window_width, cfg.window_height};
    // int map_x = cfg.map_x;
    // int map_y = cfg.map_y;

    // Uint32 start_ticks = SDL_GetTicks();
    // std::uint32_t frame_count = 0;
    // bool quit = false;
    // while (!quit)
    // {
    //     SDL_Event event;
    //     while (SDL_PollEvent(&event) != 0)
    //     {
    //         if (event.type == SDL_QUIT)
    //             quit = true;
    //         else if (event.type = SDL_KEYDOWN)
    //         {
    //             switch (event.key.keysym.sym)
    //             {
    //             case SDLK_UP:
    //                 map_y -= 8;
    //                 break;
    //             case SDLK_DOWN:
    //                 map_y += 8;
    //                 break;
    //             case SDLK_LEFT:
    //                 map_x -= 8;
    //                 break;
    //             case SDLK_RIGHT:
    //                 map_x += 8;
    //                 break;
    //             }
    //         }
    //     }

    //     SDL_RenderClear(renderer);
    //     gmap.render(map_x, map_y, fred_app.getTextureManager(), fred_app.getRenderer(), &dest);
    //     SDL_RenderPresent(renderer);

    //     ++frame_count;
    //     Uint32 frame_time = frame_count * 1000 / FRAMES_PER_SECOND;
    //     Uint32 ticks = SDL_GetTicks() - start_ticks;
    //     if (ticks < frame_time)
    //         SDL_Delay(frame_time - ticks);
    //     if (frame_count == (10 * FRAMES_PER_SECOND))
    //     {
    //         frame_count = 0;
    //         start_ticks += frame_time;
    //     }
    // }

    return 0;
}
