#include "Controller.hpp"
#include "TextureManager.hpp"

std::pair<SDL_Rect, SDL_Rect> Controller::getRects(SDL_Window *window)
{
    int window_w, window_h;
    SDL_GetWindowSize(window, &window_w, &window_h);
    int size = 0;
    if (window_w > window_h) // landscape orientation
        size = window_w / 5;
    else // portrait orientation
        size = std::min(window_w / 2, window_h / 5);
    return {SDL_Rect{0, window_h - size, size, size},
            SDL_Rect{window_w - size, window_h - size, size, size}};
}

void Controller::render(SDL_Window *window, SDL_Renderer *renderer,
                        TextureManager const &tmgr)
{
    static SDL_Rect const arrows_src{0, 0, 360, 360};
    static SDL_Rect const fire_src{390, 0, 360, 360};
    auto [arrows_dst, fire_dst] = getRects(window);
    SDL_RenderSetScale(renderer, 1.0, 1.0);
    SDL_RenderSetViewport(renderer, nullptr);
    SDL_SetTextureAlphaMod(tmgr.get(TextureID::CONTROLLER), 200);
    SDL_RenderCopy(renderer, tmgr.get(TextureID::CONTROLLER), &arrows_src, &arrows_dst);
    SDL_RenderCopy(renderer, tmgr.get(TextureID::CONTROLLER), &fire_src, &fire_dst);
}
