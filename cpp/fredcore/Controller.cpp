#include "Controller.hpp"
#include "TextureManager.hpp"
#include "DisplayConfig.hpp"

Controller::Controller(DisplayConfig const &display_cfg, bool back_button)
    : back_button(back_button)
{
    resetPosition(display_cfg);
}

void Controller::resetPosition(DisplayConfig const &display_cfg)
{
    auto [window_w, window_h] = display_cfg.getWindowSize();
    int size = 0;
    if (window_w > window_h) // landscape orientation
        size = window_w / 5;
    else // portrait orientation
        size = std::min(window_w / 2, window_h / 5);

    arrows = SDL_Rect{0, window_h - size, size, size};
    fire = SDL_Rect{window_w - size, window_h - size, size, size};
    auto third = size / 3;
    back = SDL_Rect{window_w - third, 0, third, third};

    float window_fw = static_cast<float>(window_w);
    float window_fh = static_cast<float>(window_h);
    farrows = SDL_FRect{arrows.x / window_fw, arrows.y / window_fh,
                        arrows.w / window_fw, arrows.h / window_fh};
    ffire = SDL_FRect{fire.x / window_fw, fire.y / window_fh,
                      fire.w / window_fw, fire.h / window_fh};
    fback = SDL_FRect{back.x / window_fw, back.y / window_fh,
                      back.w / window_fw, back.h / window_fh};
}

void Controller::render(SDL_Renderer *renderer,
                        TextureManager const &tmgr) const
{
    static SDL_Rect const arrows_src{0, 0, 360, 360};
    static SDL_Rect const fire_src{390, 0, 360, 360};
    static SDL_Rect const back_src{900, 120, 120, 120};
    SDL_RenderSetScale(renderer, 1.0, 1.0);
    SDL_RenderSetViewport(renderer, nullptr);
    SDL_SetTextureAlphaMod(tmgr.get(TextureID::CONTROLLER), 200);
    SDL_RenderCopy(renderer, tmgr.get(TextureID::CONTROLLER), &arrows_src, &arrows);
    SDL_RenderCopy(renderer, tmgr.get(TextureID::CONTROLLER), &fire_src, &fire);
    if (back_button)
        SDL_RenderCopy(renderer, tmgr.get(TextureID::CONTROLLER), &back_src, &back);
}
