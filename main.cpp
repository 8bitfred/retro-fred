#include <iostream>
#include "sdl.hpp"
#include "GameMap.hpp"
#include "Application.hpp"

int main(int argc, char *argv[])
{
    std::minstd_rand random_engine;
    GameMap gmap(random_engine, 17, 17);
    for (int x = 0; x < gmap.getWidth(); ++x) {
        for (int y = 0; y < gmap.getHeight(); ++y) {
            auto c = gmap.getCell(GameMap::CellPos{ x, y });
            if (c == GameMap::Cell::empty)
                std::cout << " ";
            else
                std::cout << "X";
        }
        std::cout << std::endl;
    }

    FredApp fred_app;
    auto renderer = fred_app.getRenderer();

    //TextureManager tmgr(renderer);
    //sdl::SurfacePtr surface(IMG_Load("sprites/fred_1x_01.png"));
    //SDL_SetColorKey(surface, SDL_TRUE, 0);
    //sdl::TexturePtr texture(SDL_CreateTextureFromSurface(renderer, surface));
    // sdl::texture texture(IMG_LoadTexture(renderer, "sprites/fred_1x_01c.png"));
    while (1)
    {
        SDL_Event event;
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT)
            break;

        SDL_Rect dest{14 * 8, 160, 24, 32};
        SDL_SetRenderDrawColor(renderer, 0x30, 0x40, 0x50, 0x10);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, fred_app.getTextureManager().get(TextureManager::TextureID::fred), nullptr, &dest);
        SDL_Rect dest2{14 * 8 + 6, 160, 24, 32};
        SDL_RenderCopy(renderer, fred_app.getTextureManager().get(TextureManager::TextureID::fred), nullptr, &dest2);
        SDL_RenderPresent(renderer);
    }

    return 0;
}
